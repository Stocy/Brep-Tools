#ifndef OCC_TEST_UTILS_H
#define OCC_TEST_UTILS_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <STEPControl_Writer.hxx>
#include <STEPControl_Reader.hxx>
#include <Interface_Static.hxx>
#include <Geom_Plane.hxx>

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
#include <Interface_Static.hxx>
#include <functional>

using namespace std;

/**
 * Compute stats for a TopoDS shape, bool to turn verbose on or off
 * @param shape the shape to gather stat from
 */
void Stats_TopoShapes(const TopoDS_Shape &shape, bool = true);

/**
 * Compute step files stats and store them in a csv file
 * @param folder_path folder path containing step files
 */
void StepFolder_Stats(string=string(SRCDIR)+"/step_files");

/**
 * Read a step file from a path
 * @return TopoDS_Shape shape with all roots
 */
TopoDS_Shape ReadStep(string path);

/**
 * Export TopoDS_Shape in STEP file with filename
 * @param shape
 * @param filename
 * @param unit
 */
void ExportSTEP(const TopoDS_Shape& shape, const string& filename, const string& unit);

/**
 * Get BSpline Curves in a TopoDS_Shape
 * @return vector of BSpline Curves
 */
vector<Handle(Geom_BSplineCurve)> bSC(TopoDS_Shape&,bool verbose = true);

/**
 * Get BSpline Surfaces in a TopoDS_Shape
 * @return vector of BSpline Surfaces
 */
vector<Handle(Geom_BSplineSurface)> bSS(TopoDS_Shape&,bool verbose = true);

struct taperParam {
    gp_Ax3 ax;
};

/**
 * Taper a point, according to a single argument function.
 * the argument given to the function is the height of the point in ax coordinate system
 * if shear is set to true, every point at the same height will displaced the same amount :
 * this mean the result of the taper function is the *size* of the displacement vector
 * which is then used to translate the original point accordingly.
 * Otherwise the result of the taper function is used as a factor to scale the displacement vector,
 * the height point of the point in ax is then displaced by the displacement vector.
 * @param point
 * @param ax
 * @param taperFunc
 * @param verbose
 */
void TaperPoint(gp_Pnt &point, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, bool shear = false, bool verbose = true);
/**
 * Test the taperPnt function
 * @param pnt
 * @param ax
 * @param taperFunc
 * @param tFuncFacor
 * @param verbose
 */
void TaperPoint_test(gp_Pnt &pnt, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, Standard_Real tFuncFacor, bool verbose);

/**
 * Taper a BSpline Curve (BSC), simply displace the poles of the BSpline with the help of TaperPoint using func
 * @param bSplineCurve
 * @param ax
 * @param func
 * @param verbose
 */
void TaperBSC(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> func, bool shear = false, bool verbose = false);
/**
 * Evaluate a Taper on BSpline Curve against the tapered discretization,
 * export a step file to visualize
 * @param bSplineCurve
 * @param ax
 * @param taperFunc
 * @param discr
 */
void TaperBSC_eval(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, bool shear, Standard_Integer discr = 200);
/**
 * Taper a BSpline Surface (BSS)
 * @param bSplineSurface
 * @param ax
 * @param taperFunc
 * @param verbose
 */
void TaperBSS(const Handle(Geom_BSplineSurface) &bSplineSurface, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, bool shear = false, bool verbose = false);

/**
 * Taper a shape by tapering its sub components
 * @param shape
 * @param ax
 * @param taperFunc
 * @param shear
 * @param verbose
 */
void TaperShape(TopoDS_Shape &shape, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, bool shear = false, bool verbose = false);
/**
 * DEPRECATED
 * Taper a point pnt with an angle, the CAD way,
 * should be refactored as function of taper taking point's x,y,z as argument
 * @param pnt
 * @param ax
 * @param angleRad
 * @param verbose
 */
void TaperPnt_CADStyle(gp_Pnt &pnt, gp_Ax3 &ax, Standard_Real angleRad, bool verbose);

/**
 * DEPRECATED
 * Taper a BSpline Curve (BSC), simply displace the poles of the BSpline with the help of TaperPnt_CADStyle
 * @param bSplineCurve
 * @param ax
 * @param angle_rad
 * @param verbose
 */
void TaperBSC_CADStyle(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Ax3 &ax, Standard_Real angle_rad, bool verbose);

/**
 * Evaluate a Taper on BSpline Curve against the tapered discretization
 * @param bSplineCurve
 * @param ax
 * @param angle_rad
 * @param discr
 */
void TaperBSC_eval_CADStyle(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Ax3 &ax, Standard_Real angle_rad, Standard_Integer discr);
/**
 * Taper a BSpline Surface (BSS)
 * @param bSplineSurface
 * @param ax
 * @param angle_rad
 * @param verbose
 */
void TaperBSS_CADStyle(const Handle(Geom_BSplineSurface) &bSplineSurface, gp_Ax3 &ax, Standard_Real angle_rad, bool verbose);

static bool cmp(pair<string, int>& a, pair<string, int>& b){
    return a.second > b.second;}

static vector<pair<string,int>> sort_map(map<string, int>& M){
    vector<pair<string, int> > A;
    for (auto& it : M) {
        A.push_back(it);
    }
    sort(A.begin(), A.end(), cmp);
    return A;}
void setColor(TopoDS_Shape);

#endif //OCC_TEST_UTILS_H
