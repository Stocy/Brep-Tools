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
#include "Utils.h"

using namespace std;
int main(int argc, char** argv) {
    //read step file
    //TopoDS_Shape shape = ReadStep("/home/tom/Documents/stage_can/occ_test/Cone_surf.stp");

    // compute stats
    //Stats_TopoShapes(shape);

    // export TopoDS to step file
    //ExportSTEP(shape, "out.step", "mm");

    StepFolder_Stats();
}