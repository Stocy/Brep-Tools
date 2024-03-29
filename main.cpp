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

    //Loading step files
    string stepFolder("/step_files");
    TopoDS_Shape t_curve = ReadStep(string(SRCDIR) + "/bs_curve_rational.step");
    TopoDS_Shape t_surf = ReadStep(string(SRCDIR) + "/bs_surf.step");
    TopoDS_Shape t_face = ReadStep(string(SRCDIR) + "/test_wire_closed.step");
    TopoDS_Shape tol_test = ReadStep(string(SRCDIR) + stepFolder + "/tol.step");
    TopoDS_Shape pyramid = ReadStep(string(SRCDIR) + stepFolder + "/Pyramid.stp");
    TopoDS_Shape test_cube = ReadStep(string(SRCDIR) + stepFolder + "/test_cube.step");
    TopExp_Explorer explorer(t_face, TopAbs_WIRE);
    TopoDS_Wire wire = TopoDS::Wire(explorer.Current());
    TopoDS_Shape cube = BRepPrimAPI_MakeBox(gp_Pnt(-10, 0, -10), gp_Pnt(10, 10, 10));
    TopoDS_Shape sphere = BRepPrimAPI_MakeSphere(10);
    //Stats_TopoShapes(t_curve);
    //Stats_TopoShapes(t_surf);
    vector<Handle(Geom_BSplineCurve) > bScs = bSC(t_curve, false);
    vector<Handle(Geom_BSplineSurface) > bSss = bSS(t_surf, false);

    Handle(Geom_BSplineCurve) a_bSC(bScs.at(0));
    Handle(Geom_BSplineSurface) a_bSS(bSS(t_surf).at(0));

    cout << "bs is " << (a_bSC.IsNull() ? "null" : "not null") << endl;

    //applying tapering

    //set parameters for operation ie working plane/axis
    gp_Ax3 op_axis(gp_Pnt(-0.001, -0.001, 0), gp_Dir(0, 0, 1));

    //create taper function on the fly
    //taper function need only one argument, if more needed, use a function with more arguments
    //that will create the taper function
    //some taper function examples ...

    auto func_ex_1 = [](auto h) { return -h * 0.07; };
    auto func_ex_2 = [](auto h) { return h / 1500; };
    auto func_ex_3 = [](auto h) { return -log(h); };
    //more function in TaperFunctions namespace

    TaperParams displacementTaper{
            op_axis, SCALE, TaperFunctions::displacement(10, 10, -1)
    };

    TaperParams linear{
            op_axis, SCALE, TaperFunctions::linear(-0.1)
    };
    //evaluate taper

//    TaperBSC_eval(a_bSC, displacementTaper,1000,0);
//    TaperBSS_eval(a_bSS, linear,200,1);

    TaperWire(wire,displacementTaper,3);
    ExportSTEP(wire, "testWire.step", "mm");
//
    TaperShape_wireFrame(cube, displacementTaper, 3);
    ExportSTEP(cube, "testCube.step", "mm");

    TaperShape_wireFrame(test_cube, displacementTaper, 3);
    ExportSTEP(test_cube, "test_cube_out.step", "mm");
//
//    TaperShape(sphere, linear, 3);
//    ExportSTEP(sphere, "testSphere.step", "mm");
//
//    TaperShape(tol_test, linear, 3);
//    ExportSTEP(tol_test, "tolTest.step", "mm");
//
//    TaperShape(pyramid, linear, 3);
//    ExportSTEP(pyramid, "pyramid.step", "mm");

}

void codeExamples() {
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