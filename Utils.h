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
#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)
#define _USE_MATH_DEFINES

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


/**
 * Taper functions namespace
 */
namespace TaperFunctions{
    // create a taper function with an angle TODO not correct yet
    inline auto linear = [](Standard_Real a) {
        function<double(double)> taperFunc = [a](Standard_Real h){ return 1.0 + h * a; };
        return taperFunc;
    };

    inline auto angle = [](Standard_Real angle) {
        double x(cos(angle)), y(sin(angle)), factor(y/x);
        function<double(double)> taperFunc = [factor](Standard_Real h){ return h * factor; };
        return taperFunc;
    };

    // create a taper function knowing the distance you want to displace a point at a certain distance and height
    inline auto displacement = [](Standard_Real distance, Standard_Real height, Standard_Real displacement){
        Standard_Real factor = ((distance + abs(displacement))/distance) - 1.0;
        if(displacement < 0) factor = -factor;
        function<double(double)> taperFunc = [factor, height](Standard_Real h){
            //cout << "h " << h << ", H " << height <<endl;
            return 1.0 + (factor * (h/height)); };
        return taperFunc;
    };
};
/**
 * define different uses for value returned by the taper function f
 */
enum TAPER_TYPE{
    SCALE, //scale the xy vector of the point by f
    TRANSLATE, //translate the point by the xy vector scaled to f
    CUSTOM // not implemented, allow for other types
};

/**
 * Taper parameters
 */
struct TaperParams{
    gp_Ax3 axis;
    TAPER_TYPE type;
    function<double(double)> func;
};

/**
 * Taper a point, according to a single argument function.
 * the argument given to the function is the height of the point in axis coordinate system
 * TAPER_TYPE used should be SCALE most of the time but TRANSLATE can be used for radial shear
 * @param point
 * @param ax
 * @param taperFunc
 * @param verbose
 */
void TaperPoint(gp_Pnt &point, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType, bool verbose = true);

void TaperPoint(gp_Pnt &point, gp_Ax3 &ax, TaperParams taperParams, bool verbose = true);

/**
 * Taper a BSpline Curve (BSC), simply displace the poles of the BSpline with the help of TaperPoint using func
 * @param bSplineCurve
 * @param ax
 * @param taperFunc
 * @param verbose
 */
void TaperBSC(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType, bool verbose = false);

void TaperBSC(const Handle(Geom_BSplineCurve) &bSplineCurve, TaperParams taperParams, bool verbose = false);

/**
 * Evaluate a Taper on BSpline Curve against the tapered discretization,
 * export a step file to visualize
 * output min, max aswell as average tolerance
 * @param bSplineCurve
 * @param ax
 * @param taperFunc
 * @param discr
 */
void TaperBSC_eval(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType, Standard_Integer discr = 200);

void TaperBSC_eval(const Handle(Geom_BSplineCurve) &bSplineCurve, TaperParams taperParams, Standard_Integer discr = 200);
/**
 * Taper a BSpline Surface (BSS)
 * @param bSplineSurface
 * @param ax
 * @param taperFunc
 * @param verbose
 */
void TaperBSS(const Handle(Geom_BSplineSurface) &bSplineSurface, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType, bool verbose = false);

void TaperBSS(const Handle(Geom_BSplineSurface) &bSplineSurface, TaperParams taperParams, bool verbose = false);

/**
 * Taper a shape by tapering its sub components
 * @param shape
 * @param ax
 * @param taperFunc
 * @param shear
 * @param verbose
 */
TopoDS_Compound
TaperShape(TopoDS_Shape &shape, gp_Ax3 &ax, function<Standard_Real(Standard_Real)> taperFunc, TAPER_TYPE taperType, bool verbose = false);

TopoDS_Compound
TaperShape(TopoDS_Shape &shape, TaperParams taperParams, bool verbose = false);

void setColor(TopoDS_Shape);

#endif //OCC_TEST_UTILS_H

