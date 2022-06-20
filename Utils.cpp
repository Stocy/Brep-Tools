#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAPI_IntCS.hxx>
#include <Geom_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Lin.hxx>
#include "Utils.h"

#define TOL 0.000001

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

map<string,int> edgeType_count, faceType_count;

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

void Stats_TopoShapes(const TopoDS_Shape& shape, bool verbose){
    Standard_Integer count(0);
    std::vector<Geom_Surface> surfs;

    // get underlying buffer
    streambuf* orig_buf = cout.rdbuf();

    // set null
    if(!verbose)cout.rdbuf(NULL);


    cout << "--------------- COUNT OF SHAPES OF != TYPES : ---------------" << endl;
    cout << "/////////////////////////////////////////////////////////////" << endl;
    for(int shape_enum_value = 0; shape_enum_value != TopAbs_SHAPE; shape_enum_value++){
        count = 0;
        TopAbs_ShapeEnum ev = static_cast<TopAbs_ShapeEnum>(shape_enum_value);
        for(TopExp_Explorer explorer(shape, ev); explorer.More(); explorer.Next()){
            count++;
        }
        cout << "nb of " << shape_types[shape_enum_value] << " is " << count << endl;
    }

    cout << "--------------------------- FACES TYPES ---------------------" << endl;
    cout << "/////////////////////////////////////////////////////////////" << endl;
    count = 0;
    for(TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next()){
        count++;
        const opencascade::handle<Geom_Surface> &surface = BRep_Tool::Surface(TopoDS::Face((explorer.Current())));
        //cout << "face " << count << " , continuity " << faceContinuity[surface->Continuity()] << endl;
        //cout << typeid(surface).name() << endl;
        Standard_CString type = surface->DynamicType()->Name();
        cout << "face " << count << " : "<< type << endl;
        faceType_count.contains(type) ? faceType_count[type] += 1 : faceType_count[type] = 1;
    }

    cout << "------------------ EDGES TYPES ------------------------------" << endl;
    cout << "/////////////////////////////////////////////////////////////" << endl;
    count = 0;
    for(TopExp_Explorer explorer(shape, TopAbs_EDGE); explorer.More(); explorer.Next()){
        count++;
        Standard_Real first(0.0), last(1.0);
        const opencascade::handle<Geom_Curve> &curve = BRep_Tool::Curve(TopoDS::Edge(explorer.Current()),first,last);
        if(!curve.IsNull()){
            Standard_CString type = curve->DynamicType()->Name();
            cout << "edge " << count << " : "<< type << endl;
            edgeType_count.contains(type) ? edgeType_count[type] += 1 : edgeType_count[type] = 1;
        }
    }

    cout << "--------------- COUNT OF TYPES  : ---------------------------" << endl;
    cout << "/////////////////////////////////////////////////////////////" << endl;
    cout << "--------------------------- FACES ---------------------------" << endl;
    for (auto it = faceType_count.begin(); it!= faceType_count.end(); ++it){
        cout << it->first << " " << it->second << endl;
    }
    cout << "--------------------------- EDGES ---------------------------" << endl;
    for (auto it = edgeType_count.begin(); it!= edgeType_count.end(); ++it){
        cout << it->first << " " << it->second << endl;
    }
    cout.rdbuf(orig_buf);
}

namespace fs = std::filesystem;

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
    for(auto& p: fs::directory_iterator(folder_path))++file_count;

    for(auto& p: fs::directory_iterator(folder_path)){
        cout << p.path().filename() << '\n';
        Stats_TopoShapes(ReadStep(p.path()), false);
    }
    map<string,int> totFace, totEdge;
    totFace.insert(faceType_count.begin(),faceType_count.end());
    totEdge.insert(edgeType_count.begin(),edgeType_count.end());


    //storing stats
    string stat_file(string(SRCDIR)+"/step_folder_stat.csv");
    stats.open(stat_file);

    string firstLine ("File/Geom_TYPE,");
    for(auto it = totFace.begin(); it != totFace.end(); ++it) firstLine += string(it->first) + ",";
    for(auto it = totEdge.begin(); it != totEdge.end(); ++it) firstLine += string(it->first) + ",";

    firstLine.pop_back();
    stats << firstLine << endl;

    int i=0;
    for(auto& p: fs::directory_iterator(folder_path)){
        for (auto it = faceType_count.begin(); it!=faceType_count.end(); ++it) it->second=0;
        for (auto it = edgeType_count.begin(); it!=edgeType_count.end(); ++it) it->second=0;
        Stats_TopoShapes(ReadStep(p.path()), false);
        //string line( string(p.path().filename()) + ",");
        std::ostringstream file_number;
        file_number << std::setw(to_string(file_count).size() + 1) << std::setfill('0') << ++i;
        string line( "F" +file_number.str() + ",");
        for (auto it = faceType_count.begin(); it!=faceType_count.end(); ++it) line += to_string(it->second) + ",";
        for (auto it = edgeType_count.begin(); it!=edgeType_count.end(); ++it) line += to_string(it->second) + ",";
        line.pop_back();
        stats << line << endl;
    }

    string line( "TOTAL,");
    for(auto it = totFace.begin(); it != totFace.end(); ++it) line += to_string(it->second) + ",";
    for(auto it = totEdge.begin(); it != totEdge.end(); ++it) line += to_string(it->second) + ",";
    line.pop_back();
    stats << line << endl;

    stats.close();
    cout << "------------------------------------------------------" << endl;
    cout << "Stats writen to :\n" << stat_file << endl;
}

void ExportSTEP(const TopoDS_Shape& shape, const string& filename, const string& unit) {
    cout << "------------------ exporting STEP in "<< filename << "--------------------"<< endl;
    if (shape.IsNull()) {
        throw new invalid_argument("Can't export null shape to STEP");
    }
    STEPControl_Writer writer;
    Interface_Static::SetCVal ("xstep.cascade.unit", unit.c_str());
    Interface_Static::SetCVal ("write.step.unit", unit.c_str ());
    Interface_Static::SetIVal ("write.step.nonmanifold", 1);
    // "Transfer" = convert
    IFSelect_ReturnStatus transferStatus = writer.Transfer(shape, STEPControl_AsIs);

    if (transferStatus != IFSelect_RetDone) {
        throw std::logic_error ("Error while transferring shape to STEP");
    }
    // Write transferred structure to STEP file
    IFSelect_ReturnStatus writeStatus = writer.Write(filename.c_str());

    // Return previous locale
    if (writeStatus != IFSelect_RetDone)
    {
        throw std::logic_error ("Error while writing transferred shape to STEP file");
    }
}


//void taper(const TopoDS_Shape &shape, opencascade::handle<Geom_Plane> &plane, Standard_Real angle) {}

/**
 * apply tapering to a bSpline, modify the input bSpline
 * @param bSplineCurve
 * @param ax
 * @param angle_rad
 */
void taper(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Pnt &ax, Standard_Real angle_rad) {


}

void taper(gp_Pnt &pnt, gp_Ax3 &ax, Standard_Real angle_rad) {
    gp_Vec normal_vec(ax.Direction().XYZ());
    gp_Pnt op_origin = ax.Location();
    gp_Vec point_vec_u(op_origin, pnt);

    cout << "////////////////////////////" <<endl;
    cout << "point :" << pnt.Coord().X() << " " << pnt.Coord().Y() << " " << pnt.Coord().Z() << endl;
    //cout << "vec_op " << point_vec_u.X() <<" "<< point_vec_u.Y() <<" "<< point_vec_u.Z() << endl;
    //cout << "vecZ_op " << normal_vec.X() <<" "<< normal_vec.Y() <<" "<< normal_vec.Z() << endl;
    cout << "dot_p n x u :" << point_vec_u.Dot(normal_vec) << endl;
    Standard_Real height = point_vec_u.Dot(normal_vec) / (pow(normal_vec.Magnitude(), 2));
    cout << "height :" << height << endl;

    gp_Lin lin(op_origin, normal_vec);
    gp_Pnt new_pt(pnt);

    if(height != 0 && !lin.Contains(pnt, TOL)){

        //Defining a rotative_line to rotate about operation axis
        gp_Ax3 ax_pt(op_origin, point_vec_u);
        gp_Vec rotate_vec(point_vec_u);
        rotate_vec.Cross(normal_vec);
        gp_Ax1 rotate_axis(op_origin, rotate_vec);
        ax_pt.Rotate(rotate_axis, angle_rad);
        Handle(Geom_Line) rotative_line = new Geom_Line(op_origin, ax_pt.Direction().XYZ());

        //Defining a plane with new point height in operation axis coordinates
        Handle(Geom_Plane) plane = new Geom_Plane(pnt, normal_vec);

        //Computing intersection between rotative_line and plane to find new point position
        GeomAPI_IntCS intCs(rotative_line, plane);
        if (intCs.IsDone() && intCs.NbPoints()>0) {
            new_pt = intCs.Point(1);
            cout << "new point :" << new_pt.Coord().X() <<" "<< new_pt.Coord().Y() <<" "<< new_pt.Coord().Z() << endl;
        }
    }
    else{
        cout << "point not moved" << endl;
    }
    pnt = new_pt;
}

void taper(const opencascade::handle<Geom_BSplineCurve> &bSplineCurve, gp_Ax3 &ax, Standard_Real angle_rad) {
    TColgp_Array1OfPnt poles = bSplineCurve->Poles(), new_poles(1,poles.Size());

    //displacing every control points
    Standard_Integer count(1);
    for (auto current_pole : poles){
        gp_Pnt new_pole(current_pole);
        taper(new_pole, ax, angle_rad);
        new_poles[count] = new_pole;
        count++;
    }
    count--;

    Geom_BSplineCurve res(*bSplineCurve);
    for (int i = 1; i <= count; ++i) {
        res.SetPole(i,new_poles[i]);
    }
    *bSplineCurve = res;

}

void taper(const opencascade::handle<Geom_BSplineSurface> &bSplineSurface, gp_Ax3 &ax, Standard_Real angle_rad) {
   TColgp_Array2OfPnt poles(bSplineSurface->Poles()), new_poles(1,poles.NbRows(),1,poles.NbColumns());

    for (int i = 0; i < poles.NbRows(); ++i) {
        for (int j = 0; j < poles.NbColumns(); ++j) {
            gp_Pnt new_pole(poles[i][j]);
            taper(new_pole,ax,angle_rad);
            new_poles[i][j] = new_pole;
        }
    }
    bSplineSurface->Poles(new_poles);
}
