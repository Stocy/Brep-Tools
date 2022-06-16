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
#include "Utils.h"
#include "BRepBuilderAPI.hxx"

using namespace std;
int main(int argc, char** argv) {
    //read step file
    //TopoDS_Shape shape = ReadStep("/home/tom/Documents/stage_can/occ_test/Cone_surf.stp");

    // compute stats
    //Stats_TopoShapes(shape);

    // export TopoDS to step file
    //ExportSTEP(shape, "out.step", "mm");

    //StepFolder_Stats();

    //Loading bspline from step file
    TopoDS_Shape shape = ReadStep("/home/tom/Documents/stage_can/occ_test/bs_curve.step");
    Stats_TopoShapes(shape);
    TopoDS_Edge bs_edge;
    Handle(Geom_BSplineCurve) a_bs;
    cout << "bs is " << (a_bs.IsNull()?"null":"not null") << endl;

    for(TopExp_Explorer explorer(shape, TopAbs_EDGE); explorer.More(); explorer.Next()){
        Standard_Real first(0.0), last(1.0);
        bs_edge = TopoDS::Edge(explorer.Current());
        const opencascade::handle<Geom_Curve> &curve = BRep_Tool::Curve(bs_edge,first,last);
        if(!curve.IsNull()){
            if (curve->IsInstance(Standard_Type::Instance<Geom_BSplineCurve>())){
                a_bs = GeomConvert::CurveToBSplineCurve(curve);
                cout << "bs is " << (a_bs.IsNull()?"null":"not null") << endl;
                cout << "bspline with " << a_bs->NbPoles() << " poles : " << endl;
                for (auto pole : a_bs->Poles()){
                    cout << "pole " << pole.Coord().X() <<" "<< pole.Coord().Y() <<" "<< pole.Coord().Z() << endl;
                }
            }
        }
    }

    //applying tapering
    if (!a_bs.IsNull()){
        //set parameters for operation ie working plane
        gp_Ax3 op_axis(gp_Pnt(5,5,0),gp_Dir(0,1,0));
        Handle(Geom_Plane) pl = new Geom_Plane(op_axis);
        taper(a_bs, pl,numbers::pi/100);

        //exporting result
        BRepBuilderAPI_MakeEdge apiMakeEdge(a_bs);
        ExportSTEP(apiMakeEdge.Shape(), "out.step", "mm");
        AIS_Shape aisShape(apiMakeEdge.Shape());
        aisShape.Attributes()->SetDiscretisation(1000);
        aisShape.Shape();
        //BRepMesh_ShapeVisitor bRepMeshShapeVisitor();
        //bRepMeshShapeVisitor().Visit(bs_edge);
        //auto a = bRepMeshShapeVisitor().This();
        //bRepMeshShapeVisitor().

        //cout << "aloo " << a->DynamicType() << endl;
    }
    int discr = 100;
    
    for (int i = 0; i < discr; ++i) {

    }
    //a_bs->LocalValue();
    //GeomAPI_Interpolate interpolate;


}