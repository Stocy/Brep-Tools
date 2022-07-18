#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAPI_IntCS.hxx>
#include <Geom_Curve.hxx>
#include <gp_Lin.hxx>
#include <TopoDS_Edge.hxx>
#include <GeomConvert.hxx>
#include <TopoDS_Face.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRep_Builder.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include "BRepBuilderAPI_MakeVertex.hxx"
#include "TopoDS_Builder.hxx"
#include "TopoDS_Shape.hxx"
#include "Utils.h"
#include "Geom_Geometry.hxx"
#include <Geom_BSplineSurface.hxx>
#include "Geom_Surface.hxx"
#include "Geom_Curve.hxx"
#include <gp_Sphere.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include "GeomConvert_ApproxSurface.hxx"
#include "GeomConvert_ApproxCurve.hxx"
#include <BRepTools_WireExplorer.hxx>

#define TOL 0.0001

namespace fs = std::filesystem;
static const char *shape_types[] = {
        "TopAbs_COMPOUND",
        "TopAbs_COMPSOLID",
        "TopAbs_SOLID",
        "TopAbs_SHELL",
        "TopAbs_FACE",
        "TopAbs_WIRE",
        "TopAbs_EDGE",
        "TopAbs_VERTEX",
        "TopAbs_SHAPE"
};

static const char *faceContinuity[] = {
        "GeomAbs_C0",
        "GeomAbs_G1",
        "GeomAbs_C1",
        "GeomAbs_G2",
        "GeomAbs_C2",
        "GeomAbs_C3",
        "GeomAbs_CN"
};

map<string, int> edgeType_count, faceType_count;

/**
 * Compute stats for a TopoDS shape, bool to turn verbose on or off
 * @param shape the shape to gather stat from
 */
void Stats_TopoShapes(const TopoDS_Shape &shape, int verboseLevel) {
    Standard_Integer count(0);
    std::vector<Geom_Surface> surfs;
    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;
    if (verboseLevel>0) cout << "--------------- COUNT OF SHAPES OF != TYPES : ---------------" << endl;
    if (verboseLevel>0) cout << "/////////////////////////////////////////////////////////////" << endl;
    for (int shape_enum_value = 0; shape_enum_value != TopAbs_SHAPE; shape_enum_value++) {
        count = 0;
        TopAbs_ShapeEnum ev = static_cast<TopAbs_ShapeEnum>(shape_enum_value);
        for (TopExp_Explorer explorer(shape, ev); explorer.More(); explorer.Next()) {
            count++;
        }
        if (verboseLevel>0) cout << "nb of " << shape_types[shape_enum_value] << " is " << count << endl;
    }

    if (verboseLevel>0) cout << "--------------------------- FACES TYPES ---------------------" << endl;
    if (verboseLevel>0) cout << "/////////////////////////////////////////////////////////////" << endl;
    count = 0;
    for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next()) {
        count++;
        const opencascade::handle<Geom_Surface> &surface = BRep_Tool::Surface(TopoDS::Face((explorer.Current())));
        //if (verboseLevel>0) cout << "face " << count << " , continuity " << faceContinuity[surface->Continuity()] << endl;
        //if (verboseLevel>0) cout << typeid(surface).name() << endl;
        Standard_CString type = surface->DynamicType()->Name();
        if (verboseLevel>0) cout << "face " << count << " : " << type << endl;
        faceType_count.contains(type) ? faceType_count[type] += 1 : faceType_count[type] = 1;
    }

    if (verboseLevel>0) cout << "------------------ EDGES TYPES ------------------------------" << endl;
    if (verboseLevel>0) cout << "/////////////////////////////////////////////////////////////" << endl;
    count = 0;
    for (TopExp_Explorer explorer(shape, TopAbs_EDGE); explorer.More(); explorer.Next()) {
        count++;
        Standard_Real first(0.0), last(1.0);
        const opencascade::handle<Geom_Curve> &curve = BRep_Tool::Curve(TopoDS::Edge(explorer.Current()), first, last);
        if (!curve.IsNull()) {
            Standard_CString type = curve->DynamicType()->Name();
            if (verboseLevel>0) cout << "edge " << count << " : " << type << endl;
            edgeType_count.contains(type) ? edgeType_count[type] += 1 : edgeType_count[type] = 1;
        }
    }

    if (verboseLevel>0) cout << "--------------- COUNT OF TYPES  : ---------------------------" << endl;
    if (verboseLevel>0) cout << "/////////////////////////////////////////////////////////////" << endl;
    if (verboseLevel>0) cout << "--------------------------- FACES ---------------------------" << endl;
    for (auto it = faceType_count.begin(); it != faceType_count.end(); ++it) {
        if (verboseLevel>0) cout << it->first << " " << it->second << endl;
    }
    if (verboseLevel>0) cout << "--------------------------- EDGES ---------------------------" << endl;
    for (auto it = edgeType_count.begin(); it != edgeType_count.end(); ++it) {
        if (verboseLevel>0) cout << it->first << " " << it->second << endl;
    }
}

/**
 * Compute step files stats and store them in a csv file
 * @param folder_path folder path containing step files
 */
void StepFolder_Stats(string folder_path) {
    //init
    cout << "------------------------------------------------------" << endl;
    cout << "Computing stats from step files in folder : " << endl;
    cout << folder_path << endl;
    std::ofstream stats;
    faceType_count.clear();
    edgeType_count.clear();

    //computing stats
    cout << "------------------------------------------------------" << endl;
    int file_count(0);
    for (auto &p: fs::directory_iterator(folder_path))++file_count;

    for (auto &p: fs::directory_iterator(folder_path)) {
        cout << p.path().filename() << '\n';
        Stats_TopoShapes(ReadStep(p.path()), 0);
    }
    map<string, int> totFace, totEdge;
    totFace.insert(faceType_count.begin(), faceType_count.end());
    totEdge.insert(edgeType_count.begin(), edgeType_count.end());


    //storing stats
    string stat_file(string(SRCDIR) + "/step_folder_stat.csv");
    stats.open(stat_file);

    string firstLine("File/Geom_TYPE,");
    for (auto it = totFace.begin(); it != totFace.end(); ++it) firstLine += string(it->first) + ",";
    for (auto it = totEdge.begin(); it != totEdge.end(); ++it) firstLine += string(it->first) + ",";

    firstLine.pop_back();
    stats << firstLine << endl;

    int i = 0;
    for (auto &p: fs::directory_iterator(folder_path)) {
        for (auto it = faceType_count.begin(); it != faceType_count.end(); ++it) it->second = 0;
        for (auto it = edgeType_count.begin(); it != edgeType_count.end(); ++it) it->second = 0;
        Stats_TopoShapes(ReadStep(p.path()), 0);
        //string line( string(p.path().filename()) + ",");
        std::ostringstream file_number;
        file_number << std::setw(to_string(file_count).size() + 1) << std::setfill('0') << ++i;
        string line("F" + file_number.str() + ",");
        for (auto it = faceType_count.begin(); it != faceType_count.end(); ++it) line += to_string(it->second) + ",";
        for (auto it = edgeType_count.begin(); it != edgeType_count.end(); ++it) line += to_string(it->second) + ",";
        line.pop_back();
        stats << line << endl;
    }

    string line("TOTAL,");
    for (auto it = totFace.begin(); it != totFace.end(); ++it) line += to_string(it->second) + ",";
    for (auto it = totEdge.begin(); it != totEdge.end(); ++it) line += to_string(it->second) + ",";
    line.pop_back();
    stats << line << endl;

    stats.close();
    cout << "------------------------------------------------------" << endl;
    cout << "Stats writen to :\n" << stat_file << endl;
}

/**
 * Read a step file from a path
 * @return TopoDS_Shape shape with all roots
 */
TopoDS_Shape ReadStep(string path) {
    STEPControl_Reader reader;
    IFSelect_ReturnStatus stat = reader.ReadFile(path.c_str());
    IFSelect_PrintCount mode = IFSelect_ListByItem;
    //reader.PrintCheckLoad(false, mode);

    Standard_Integer NbRoots = reader.NbRootsForTransfer();                      //Transfer whole file
    Standard_Integer num = reader.TransferRoots();
    Standard_Integer NbTrans = reader.TransferRoots();

    TopoDS_Shape result = reader.OneShape();
    return result;
}

/**
 * Export TopoDS_Shape in STEP file with filename
 * @param shape
 * @param filename
 * @param unit
 */
void ExportSTEP(const TopoDS_Shape &shape, const string &filename, const string &unit, int verboseLevel) {
    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;
    if (verboseLevel>0) cout << "------------------ exporting STEP in " << filename << "--------------------" << endl;
    if (shape.IsNull()) {
        throw new invalid_argument("Can't export null shape to STEP");
    }
    STEPControl_Writer writer;
    Interface_Static::SetCVal("xstep.cascade.unit", unit.c_str());
    Interface_Static::SetCVal("write.step.unit", unit.c_str());
    Interface_Static::SetIVal("write.step.nonmanifold", 1);
    // "Transfer" = convert
    IFSelect_ReturnStatus transferStatus = writer.Transfer(shape, STEPControl_AsIs);

    if (transferStatus != IFSelect_RetDone) {
        throw std::logic_error("Error while transferring shape to STEP");
    }
    // Write transferred structure to STEP file
    IFSelect_ReturnStatus writeStatus = writer.Write(filename.c_str());
    // Return previous locale
    if (writeStatus != IFSelect_RetDone) {
        throw std::logic_error("Error while writing transferred shape to STEP file");
    }
}

/**
 * Get BSpline Curves in a TopoDS_Shape
 * @return vector of BSpline Curves
 */
vector<Handle(Geom_BSplineCurve) > bSC(TopoDS_Shape &shape, int verboseLevel) {
    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;
    TopoDS_Edge bs_edge;
    Handle(Geom_BSplineCurve) a_bs;
    vector<Handle(Geom_BSplineCurve) > res;
    if(verboseLevel > 0) cout << "bs is " << (a_bs.IsNull() ? "null" : "not null") << endl;

    for (TopExp_Explorer explorer(shape, TopAbs_EDGE); explorer.More(); explorer.Next()) {
        Standard_Real first(0.0), last(1.0);
        bs_edge = TopoDS::Edge(explorer.Current());
        const opencascade::handle<Geom_Curve> &curve = BRep_Tool::Curve(bs_edge, first, last);
        if (!curve.IsNull()) {
            if (curve->IsInstance(Standard_Type::Instance<Geom_BSplineCurve>())) {
                a_bs = GeomConvert::CurveToBSplineCurve(curve);
                if(verboseLevel>0) cout << "bs is " << (a_bs.IsNull() ? "null" : "not null") << endl;
                if(verboseLevel>0) cout << "bspline with " << a_bs->NbPoles() << " poles : " << endl;
                for (auto pole: a_bs->Poles()) {
                    if(verboseLevel>0) cout << "BSC pole " << pole.Coord().X() << " " << pole.Coord().Y() << " " << pole.Coord().Z() << endl;
                }
                res.push_back(a_bs);
            }
        }
    }
    return res;
}

/**
 * Get BSpline Surfaces in a TopoDS_Shape
 * @return vector of BSpline Surfaces
 */
vector<Handle(Geom_BSplineSurface) > bSS(TopoDS_Shape &shape,int verboseLevel) {

    vector<Handle(Geom_BSplineSurface) > res;
    Stats_TopoShapes(shape,verboseLevel-1);
    TopoDS_Face bs_face;
    Handle(Geom_BSplineSurface) a_bs;
    if(verboseLevel>0) cout << "bs is " << (a_bs.IsNull() ? "null" : "not null") << endl;

    for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next()) {
        Standard_Real first(0.0), last(1.0);
        bs_face = TopoDS::Face(explorer.Current());
        const opencascade::handle<Geom_Surface> &surface = BRep_Tool::Surface(bs_face);
        if (!surface.IsNull()) {
            if (surface->IsInstance(Standard_Type::Instance<Geom_BSplineSurface>())) {
                a_bs = GeomConvert::SurfaceToBSplineSurface(surface);
                if(verboseLevel>0) cout << "bs is " << (a_bs.IsNull() ? "null" : "not null") << endl;
                if(verboseLevel>0) cout << "bspline surface with " << a_bs->Poles().Size() << " poles : " << endl;
                for (int i = 1; i <= a_bs->Poles().NbRows(); ++i) {
                    for (int j = 1; j <= a_bs->Poles().NbColumns(); ++j) {
                        gp_Pnt pole = a_bs->Pole(i, j);
                        if(verboseLevel>0) cout << "BSS pole " << pole.Coord().X() << " " << pole.Coord().Y() << " " << pole.Coord().Z()
                             << endl;
                    }
                }
                res.push_back(a_bs);
            }
        }
    }
    return res;
}

void TaperPoint(gp_Pnt &point, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType, int verboseLevel) {
    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;
    gp_Vec normalVec(ax.Direction().XYZ());
    gp_Pnt opOrigin = ax.Location();
    gp_Vec pntVec(opOrigin, point);

    gp_Vec vecHeight = (pntVec.Dot(normalVec) / normalVec.Dot(normalVec)) * normalVec;
    Standard_Real height(vecHeight.Magnitude()), factor(taperFunc(height));
    if(verboseLevel>0) cout << "height : " << height << endl;
    if(verboseLevel>0) cout << "factor : " << factor << endl;
    gp_Pnt heightPnt(opOrigin.Translated(vecHeight));

    gp_Pnt newPoint;
    gp_Vec displacementVec(heightPnt, point);
    if(abs(displacementVec.Magnitude()) > TOL && abs(height) > TOL){

        if(taperType==TRANSLATE){
            displacementVec.Scale(factor / displacementVec.Magnitude());
            newPoint = point.Translated(displacementVec);
        }
        if(taperType==SCALE){
            displacementVec.Scale(factor);
            newPoint = heightPnt.Translated(displacementVec);
        }
    } else newPoint = point;

    point = newPoint;
}

void TaperPoint(gp_Pnt &point, gp_Ax3 &ax, TaperParams taperParams, int verboseLevel) {
    TaperPoint(point, taperParams.axis, taperParams.func, taperParams.type, verboseLevel);
}

void TaperBSC(const opencascade::handle<Geom_BSplineCurve> &bSplineCurve, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType,
              int verboseLevel) {

    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;
    TColgp_Array1OfPnt poles = bSplineCurve->Poles(), new_poles(1, poles.Size());

    //displacing every control points (aka poles)
    Standard_Integer count(1);
    for (auto current_pole: poles) {
        gp_Pnt new_pole(current_pole);
        TaperPoint(new_pole, ax, taperFunc, taperType, verboseLevel-1);
        new_poles[count] = new_pole;

        count++;
    }
    count--;

    Geom_BSplineCurve res(*bSplineCurve);
    for (int i = 1; i <= count; ++i) {
        res.SetPole(i, new_poles[i]);
    }
    *bSplineCurve = res;
}

void TaperBSC(const opencascade::handle<Geom_BSplineCurve> &bSplineCurve, TaperParams taperParams, int verboseLevel) {
    TaperBSC(bSplineCurve, taperParams.axis, taperParams.func, taperParams.type, verboseLevel);
}

void TaperBSC_eval(const opencascade::handle<Geom_BSplineCurve> &bSplineCurve, gp_Ax3 &ax,
                   function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType, int discr, int verboseLevel) {

    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;
    vector<Standard_Real> dists(discr + 1);
    vector<gp_Pnt> discr_pnts(discr + 1);

    TopoDS_Compound compound;
    BRep_Builder builder;
    builder.MakeCompound(compound);

    //adding operation axes adges, for better visualisation
    BRepBuilderAPI_MakeEdge *originAxes[3] = {
            new BRepBuilderAPI_MakeEdge(ax.Location(), ax.Location().Translated(ax.XDirection())),
            new BRepBuilderAPI_MakeEdge(ax.Location(), ax.Location().Translated(ax.YDirection())),
            new BRepBuilderAPI_MakeEdge(ax.Location(), ax.Location().Translated(((gp_Vec) ax.Direction()).Scaled(3)))
    };
    for (auto e: originAxes) {
        builder.Add(compound, e->Shape());
    }

    Handle(Geom_Geometry) tmp_geom = bSplineCurve->Copy();
    Handle(Geom_BSplineCurve) newCurve = Handle(Geom_BSplineCurve)::DownCast(tmp_geom);
    TaperBSC(newCurve, ax, taperFunc, taperType, verboseLevel-1);

    double sphereRadius(0.5);
    for (auto p: bSplineCurve->Poles()) {
        BRepPrimAPI_MakeSphere makeSphere(p, 0.5/2);
        builder.Add(compound, makeSphere.Shape());
    }
    for (auto p: newCurve->Poles()) {
        BRepPrimAPI_MakeSphere makeSphere(p, 0.5);
        builder.Add(compound, makeSphere.Shape());
    }

    BRepBuilderAPI_MakeEdge apiMakeEdge(newCurve);
    builder.Add(compound, apiMakeEdge.Shape());
    BRepBuilderAPI_MakeEdge apiMakeEdgeOrig(bSplineCurve);
    builder.Add(compound, apiMakeEdgeOrig.Shape());

    for (int i = 0; i <= discr; ++i) {
        Standard_Real U = (Standard_Real) i / discr;
        if (verboseLevel>0) cout << U << endl;
        gp_Pnt pnt(bSplineCurve->Value(U)), pnt_on_curve;
        TaperPoint(pnt, ax, taperFunc, taperType, verboseLevel-1);
        discr_pnts[i] = pnt;
        //GeomAPI_ProjectPointOnCurve geomApiProjectPointOnCurve(pnt_on_curve ,newCurve);
        //Standard_Real dst = geomApiProjectPointOnCurve.LowerDistance();
        ShapeAnalysis_Curve shapeAnalysisCurve;
        Standard_Real dst = 0.0, param(0.0);

        shapeAnalysisCurve.Project(newCurve, pnt, 0, pnt_on_curve, param);
        BRepBuilderAPI_MakeEdge makeEdge(pnt, pnt_on_curve);
        //if (!pnt.IsEqual(pnt_on_curve,0)) builder.Add(compound, makeEdge.Shape());
        dst = pnt_on_curve.Distance(pnt);
        dists[i] = dst;
        if (verboseLevel>0) cout << "dst to tapered bSpline : " << dst << endl;

    }
    Standard_Real min(dists[0]), max(min), sum(0);
    for (auto d: dists) {
        if (d > max) max = d;
        if (d < min) min = d;
        sum += d;
    }

    for (int i = 1; i <= bSplineCurve->NbKnots(); ++i) {
        Standard_Real U = bSplineCurve->Knot(i);
        if (verboseLevel>0) cout << U << endl;
    }
    if (verboseLevel>0) cout << bSplineCurve->FirstParameter() << endl;
    if (verboseLevel>0) cout << bSplineCurve->LastParameter() << endl;

    for (int i = 0; i < discr; ++i) {
        BRepBuilderAPI_MakeVertex vertex = BRepBuilderAPI_MakeVertex(discr_pnts[i]);
        builder.Add(compound, vertex.Vertex());
        BRepBuilderAPI_MakeEdge makeEdge(discr_pnts[i], discr_pnts[i + 1]);
    }

    ExportSTEP(compound, "bsc_verif.step", "mm", 0);
    if (verboseLevel>0) cout << "min : " << min << ", max : " << max << " average : " << sum / discr << endl;
}

void TaperBSC_eval(const opencascade::handle<Geom_BSplineCurve> &bSplineCurve, TaperParams taperParams,
                   Standard_Integer discr) {
    TaperBSC_eval(bSplineCurve, taperParams.axis, taperParams.func, taperParams.type, discr);
}

void TaperBSS(const opencascade::handle<Geom_BSplineSurface> &bSplineSurface, gp_Ax3 &ax,
              function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType,
              int verboseLevel) {
    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;
    TColgp_Array2OfPnt poles(bSplineSurface->Poles());
    for (int i = 1; i <= poles.NbRows(); ++i) {
        for (int j = 1; j <= poles.NbColumns(); ++j) {
            gp_Pnt new_pole(poles(i, j));
            TaperPoint(new_pole, ax, taperFunc,taperType,verboseLevel-1);
            bSplineSurface->SetPole(i, j, new_pole);
        }
    }

}

void TaperBSS(const Handle(Geom_BSplineSurface) &bSplineSurface, TaperParams taperParams, int verboseLevel){
    TaperBSS(bSplineSurface, taperParams.axis, taperParams.func, taperParams.type, verboseLevel);
}


void TaperEdge(TopoDS_Edge &edge, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType, int verboseLevel) {
    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;

    GeomAbs_Shape absShape = GeomAbs_C2;
    Standard_Real first(0.0), last(1.0);

    Handle(Geom_Curve) curve = BRep_Tool::Curve(edge,first,last);
    GeomConvert_ApproxCurve approxCurve(curve,TOL,absShape,10,10);
    Handle(Geom_BSplineCurve) bsCurve = approxCurve.Curve();
    TaperBSC(bsCurve, ax, taperFunc, taperType, verboseLevel - 1);

    BRepBuilderAPI_MakeEdge makeEdge(bsCurve);
    edge = makeEdge.Edge();
}

void TaperEdge(TopoDS_Edge &edge, TaperParams taperParams, int verboseLevel) {
    TaperEdge(edge,taperParams.axis,taperParams.func,taperParams.type,verboseLevel);
}

void TaperWire(TopoDS_Wire &wire, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType,
               int verboseLevel) {
    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;

    BRepTools_WireExplorer wireExplorer(wire);
    BRepBuilderAPI_MakeWire makeWire;
    while(wireExplorer.More()){
        TopoDS_Edge edge = TopoDS::Edge(wireExplorer.Current());
        TaperEdge(edge,ax,taperFunc,taperType,verboseLevel-1);
        makeWire.Add(edge);
        wireExplorer.Next();
    }
    if (makeWire.IsDone()) wire = makeWire.Wire();
}

void TaperFace(TopoDS_Face &face, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType,
               int verboseLevel) {
    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;

    GeomAbs_Shape absShape = GeomAbs_C2;

    const Handle(Geom_Surface) &surface = BRep_Tool::Surface(TopoDS::Face(face));
    GeomConvert_ApproxSurface approxSurface(surface,TOL,absShape,absShape,10,10,10,1);
    Handle(Geom_BSplineSurface) bsSurface = approxSurface.Surface();
    TaperBSS(bsSurface,ax,taperFunc,taperType,verboseLevel-1);

    ShapeAnalysis_FreeBounds freeBounds = ShapeAnalysis_FreeBounds(face);
    TopoDS_Compound wires = freeBounds.GetClosedWires();
    TopExp_Explorer explorerWire(wires, TopAbs_WIRE);
    TopoDS_Wire wire(TopoDS::Wire(explorerWire.Current())), newWire;
    TaperWire(wire,ax,taperFunc,taperType,verboseLevel-1);

    BRepBuilderAPI_MakeFace makeFace = BRepBuilderAPI_MakeFace(bsSurface, wire);
    face = makeFace.Face();

}

void TaperShape(TopoDS_Shape &shape, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc,
                TAPER_TYPE taperType, int verboseLevel) {
    if (verboseLevel>0) cout << "function " << __FUNCTION__ << endl;

    TopoDS_Compound compound;
    BRep_Builder builder;
    builder.MakeCompound(compound);

    for (TopExp_Explorer faceExplorer(shape, TopAbs_FACE); faceExplorer.More(); faceExplorer.Next()) {
        TopoDS_Face face = TopoDS::Face(faceExplorer.Current());
        TaperFace(face,ax,taperFunc,taperType,verboseLevel-1);
        builder.Add(compound,face);
    }

    shape = compound;
}

void TaperShape(TopoDS_Shape &shape, TaperParams taperParams, int verboseLevel) {
    TaperShape(shape, taperParams.axis, taperParams.func, taperParams.type, verboseLevel);
}

void setColor(TopoDS_Shape shape) {
    Handle(XCAFDoc_ShapeTool) shapeTool;
    shapeTool->Init();
    shapeTool->AddShape(shape);
    // TODO setColor(shape);

}
