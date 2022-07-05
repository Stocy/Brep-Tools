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
#include "Utils.h"
#include "BRepBuilderAPI.hxx"
#include "TopoDS_Vertex.hxx"
//#include <occutils/ExtendedSTEP.hxx>
//#include <occutils/Primitive.hxx>

using namespace std;

int main(int argc, char **argv) {
    //read step file
    //TopoDS_Shape t_curve = ReadStep("/home/tom/Documents/stage_can/occ_test/Cone_surf.stp");

    // compute stats
    //Stats_TopoShapes(t_curve);

    // export TopoDS to step file
    //ExportSTEP(t_curve, "out.step", "mm");

    //StepFolder_Stats();

    //Loading bspline from step file
    TopoDS_Shape t_curve = ReadStep(string(SRCDIR) + "/bs_curve_rational.step");
    TopoDS_Shape t_surf = ReadStep(string(SRCDIR) + "/bs_surf.step");

    //Stats_TopoShapes(t_curve);
    //Stats_TopoShapes(t_surf);
    vector<Handle(Geom_BSplineCurve) > bScs = bSC(t_curve, false);
    vector<Handle(Geom_BSplineSurface) > bSss = bSS(t_surf, false);

    Handle(Geom_BSplineCurve) a_bSC(bScs.at(0));
    Handle(Geom_BSplineSurface) a_bSS(bSS(t_surf).at(0));

    cout << "bs is " << (a_bSC.IsNull() ? "null" : "not null") << endl;

    //applying tapering
    if (!a_bSC.IsNull()) {

        //set parameters for operation ie working plane
        gp_Ax3 op_axis(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0));
        //TaperPnt_CADStyle(a_bSC, op_axis, -numbers::pi / 100, true);
        //TaperPnt_CADStyle(a_bSS, op_axis, -numbers::pi / 100, true);


        /*
        TopoDS_Compound pts;
        BRep_Builder builder;
        builder.MakeCompound(pts);
        for (gp_Pnt &pole : a_bSC->Poles()){
            TaperPnt_CADStyle(pole,op_axis, [](auto r){return 0.05*r;},1,true);
            BRepBuilderAPI_MakeVertex vertexBuilder = BRepBuilderAPI_MakeVertex(pole);
            TopoDS_Shape vertex = vertexBuilder.Vertex();
            builder.Add(pts, vertex);
        }

        ExportSTEP(pts,"pnts_bsc.step","mm");
         */




        //exporting result
        //BRepBuilderAPI_MakeEdge apiMakeEdge(a_bSC);
        //BRepBuilderAPI_MakeFace apiMakeFace;
        //apiMakeFace.Init(a_bSS,true,0.00001);
        //ExportSTEP(apiMakeEdge.Shape(), "out_bsc.step", "mm");
        //ExportSTEP(apiMakeFace.Shape(), "out_bss.step", "mm");
        auto func_angle = [](auto h) {
            Standard_Real angle = 0.1;
            //TODO line with the angle with function that create a function
            //gp_Vec o, p(cos(angle), sin(angle));
        };
        auto func = [](auto h) { return -h * 0.07; };
        auto func_2 = [](auto h) { return -h/300; };
        auto func_exp = [](auto h) { return -h * h * 0.1; };
        TaperBSC_eval(a_bSC, op_axis, func_2, 200);
        cout << "heheeeee" << endl;
        //a_bSC->MovePoint()

        //STEP::ExtendedSTEPExporter stepExporter;
        //stepExporter.AddShapeWithColor(cube, Quantity_NOC_RED);
        //stepExporter.Write("ColoredCube.step");

    }

    //a_bSC->LocalValue();
    //GeomAPI_Interpolate interpolate;


}