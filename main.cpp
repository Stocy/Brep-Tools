#include <iostream>
#include <Standard_Handle.hxx>
#include <StepToGeom.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <vector>
#include <TopoDS_Shape.hxx>

#include <map>
#include <filesystem>
#include <Geom_BSplineCurve.hxx>
#include <GeomConvert.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <AIS_Shape.hxx>
#include <BRepMesh_ShapeVisitor.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_BSplineSurface.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Builder.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <functional>
#include "Utils.h"
#include "BRepBuilderAPI.hxx"
#include "TopoDS_Vertex.hxx"
#include <BRepPrimAPI_MakeBox.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
//#include <occutils/ExtendedSTEP.hxx>
//#include <occutils/Primitive.hxx>

using namespace std;

int main(int argc, char **argv) {

    //Loading bspline from step file
    string stepFolder("/step_files");
    TopoDS_Shape t_curve = ReadStep(string(SRCDIR) + "/bs_curve_rational.step");
    TopoDS_Shape t_surf = ReadStep(string(SRCDIR) + "/bs_surf.step");
    TopoDS_Shape t_face = ReadStep(string(SRCDIR) + "/test_wire_closed.step");
    TopoDS_Shape tol_test = ReadStep(string(SRCDIR) +stepFolder+ "/tol.step");
//    TopoDS_Shape cone_surf = ReadStep(string(SRCDIR) +stepFolder+ "/Cones_surf.stp");
    TopoDS_Shape pyramid = ReadStep(string(SRCDIR) +stepFolder+ "/Pyramid.stp");
    TopExp_Explorer explorer(t_face,TopAbs_WIRE);
    TopoDS_Wire wire = TopoDS::Wire(explorer.Current());
    TopoDS_Shape cube = BRepPrimAPI_MakeBox(gp_Pnt(-10,-10,-10),gp_Pnt(10,10,10));
    TopoDS_Shape sphere = BRepPrimAPI_MakeSphere(10);
    //Stats_TopoShapes(t_curve);
    //Stats_TopoShapes(t_surf);
    vector<Handle(Geom_BSplineCurve)> bScs = bSC(t_curve, false);
    vector<Handle(Geom_BSplineSurface)> bSss = bSS(t_surf, false);

    Handle(Geom_BSplineCurve) a_bSC(bScs.at(0));
    Handle(Geom_BSplineSurface) a_bSS(bSS(t_surf).at(0));

    cout << "bs is " << (a_bSC.IsNull() ? "null" : "not null") << endl;

    //applying tapering
    if (!a_bSC.IsNull()) {

        //set parameters for operation ie working plane/axis
        gp_Ax3 op_axis(gp_Pnt(-0.001, -0.001, 0), gp_Dir(0, 1, 0));

        //create taper function on the fly
        //taper function need only one argument, if more needed, use a function with more arguments
        //that will create the taper function
        //some taper function examples ...
        auto func_ex_1 = [](auto h) { return -h * 0.07; };
        auto func_ex_2 = [](auto h) { return h/1500; };
        auto func_ex_3 = [](auto h) { return -log(h); };
        //more function in TaperFunctions namespace

        TaperParams displacementTaper{
            op_axis,SCALE,TaperFunctions::displacement(20.0, 20.0, -4)
        };

        TaperParams linear{
            op_axis,SCALE,TaperFunctions::linear(-0.01)
        };
        //evaluate taper

//        TaperBSC_eval(a_bSC, displacementTaper,1000,0);
//        TaperBSS_eval(a_bSS, linear,200,1);

        //ExportSTEP(cube,"cube.step","mm");
        //ExportSTEP(cube,"testCube.step","mm");
        //cout << "before" << endl;
        //Stats_TopoShapes(cube);

        //BRepBuilderAPI_MakeEdge makeEdge(a_bSC);
        //TopoDS_Edge edge = makeEdge.Edge();
        //TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge);
        //TaperEdge(edge,displacementTaper,0);
        //ExportSTEP(edge,"edge.step","mm");

        //TaperWire(wire,displacementTaper,0);
        //ExportSTEP(edge,"wire.step","mm");

        //tBuilder.MakeCompound(topoDsCompound);
        TopExp_Explorer edgeExpl(t_curve,TopAbs_EDGE);

        TopoDS_Edge bs_edge = TopoDS::Edge(edgeExpl.Current());
        TaperEdge(bs_edge,displacementTaper,3);
        TaperWire(wire,displacementTaper,3);
        ExportSTEP(bs_edge,"bsEdge.step","mm");
        ExportSTEP(wire,"tapered_wire.step","mm");
        TaperShape(cube,linear,3);
        TaperShape(sphere,linear,3);
        TaperShape(tol_test,linear,3);
        //TaperShape(cone_surf,linear,3);
        TaperShape(pyramid,linear,3);
        //cout << "after" << endl;
        //Stats_TopoShapes(cube);
        //cout << (a.IsNull()?"y":"n") << endl;
        //ExportSTEP(topoDsCompound,"wire_test.step","mm");
       ExportSTEP(cube,"testCube.step","mm");
       ExportSTEP(sphere,"testSphere.step","mm");
        ExportSTEP(tol_test,"tolTest.step","mm");
        ExportSTEP(pyramid,"pyramid.step","mm");
        //ExportSTEP(cone_surf,"coneSurf.step","mm");
    }

}

void codeExamples(){
    //CODE EXAMPLES ...

    //read step file
    TopoDS_Shape t_curve = ReadStep("/home/tom/Documents/stage_can/occ_test/Cone_surf.stp");

    //compute stats
    Stats_TopoShapes(t_curve);

    //compute folder stats
    StepFolder_Stats();

    // export TopoDS to step file
    ExportSTEP(t_curve, "out.step", "mm");
}