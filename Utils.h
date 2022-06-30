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

struct taperParam {
    gp_Ax3 ax;
};

void TaperPnt(gp_Pnt &pnt, gp_Ax3 &ax, Standard_Real angle_rad, bool verbose);
void TaperPnt(gp_Pnt &pnt, gp_Ax3 &ax, Standard_Real (*taperFunc)(Standard_Real), bool verbose);
void TaperPnt_test(gp_Pnt &pnt, gp_Ax3 &ax, Standard_Real (*taperFunc)(Standard_Real), Standard_Real tFuncFacor, bool verbose);

void TaperBSC(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Ax3 &ax, Standard_Real angle_rad, bool verbose);
void TaperBSC(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Ax3 &ax, Standard_Real(*func)(Standard_Real), bool verbose);
void TaperBSC_eval(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Ax3 &ax, Standard_Real angle_rad, Standard_Integer discr);
void TaperBSC_eval(const Handle(Geom_BSplineCurve) &bSplineCurve, gp_Ax3 &ax, Standard_Real(*func)(Standard_Real), Standard_Integer discr);

void TaperBSS(const Handle(Geom_BSplineSurface) &bSplineSurface, gp_Ax3 &ax, Standard_Real angle_rad, bool verbose);
void TaperBSS(const Handle(Geom_BSplineSurface) &bSplineSurface, gp_Ax3 &ax, Standard_Real(*func)(Standard_Real), bool verbose);
void setColor(TopoDS_Shape);

vector<Handle(Geom_BSplineSurface)> bSS(TopoDS_Shape&);
vector<Handle(Geom_BSplineCurve)> bSC(TopoDS_Shape&);

static bool cmp(pair<string, int>& a, pair<string, int>& b){
    return a.second > b.second;}
static vector<pair<string,int>> sort_map(map<string, int>& M){
    vector<pair<string, int> > A;
    for (auto& it : M) {
        A.push_back(it);
    }
    sort(A.begin(), A.end(), cmp);
    return A;}

#endif //OCC_TEST_UTILS_H
