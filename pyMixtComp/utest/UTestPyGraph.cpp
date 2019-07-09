/* MixtComp version 4  - july 2019
 * Copyright (C) Inria - Université de Lille - CNRS */

/*
 *  Project:    MixtComp
 *  Created on: June 21, 2019
 *  Author:     Quentin GRIMONPREZ <quentin.grimonprez@inria.fr>
 **/

#include "gtest/gtest.h"
#include "MixtComp.h"
#include "pythonIO.h"
#include <map>


using namespace mixt;

TEST(PyGraph, constructor) {
	Py_Initialize();
	PyGraph g;

	boost::python::dict d = g.get();
	boost::python::dict d0;

	EXPECT_EQ(d, d0);

	d0["toto"] = 3;
	PyGraph g2(d0);
	d = g2.get();

	EXPECT_EQ(d, d0);
}

TEST(PyGraph, basicAdd0) {
	Py_Initialize();
	PyGraph g;
	g.add_payload( { }, "toto", 12);
	boost::python::dict comp = g.get();
	boost::python::dict exp;
	exp["toto"] = 12;
	ASSERT_EQ(comp, exp);
}

TEST(PyGraph, basicAdd1) {
	PyGraph g;
	g.add_payload( { "complex", "path" }, "toto", 12);
	boost::python::dict comp = g.get();

	boost::python::dict exp;
	exp["complex"] = boost::python::dict();
	exp["complex"]["path"] = boost::python::dict();
	exp["complex"]["path"]["toto"] = 12;

	ASSERT_EQ(exp, comp);
}


TEST(PyGraph, basicGet) {
	Index exp = 12;
	PyGraph g;
	g.add_payload( { "complex", "path" }, "toto", exp);
	Index comp;
	g.get_payload( { "complex", "path" }, "toto", comp);
	ASSERT_EQ(comp, exp);
}

TEST(PyGraph, combined) {
	boost::python::dict exp;
	exp["paramStr"] = "A parameter";
	exp["varA"] = boost::python::dict();
	exp["varA"]["A nested real"] = 12.0;
	exp["varA"]["A nested string"] = "Hello World !";

	PyGraph g;
	g.add_payload( { }, "paramStr", "A parameter");
	g.add_payload( { "varA" }, "A nested real", 12.0);
	g.add_payload( { "varA" }, "A nested string", "Hello World !");

	boost::python::dict comp = g.get();

	ASSERT_EQ(exp, comp);
}

//TEST(PyGraph, NamedVectorReal) {
//	std::string exp = R"-({"A named vector":{"ctype":"Vector","data":[1.0,2.0,3.0],"dtype":"Real","nrow":3,"rowNames":["riri","fifi","loulou"]}})-";
//	PyGraph gIn;
//	gIn.set(exp);
//
//	Vector<Real> vec;
//	std::vector<std::string> rowNames;
//	NamedVector<Real> nv = { rowNames, vec };
//	gIn.get_payload( { }, "A named vector", nv);
//
//	PyGraph gOut;
//	gOut.add_payload( { }, "A named vector", nv);
//	std::string comp = gOut.get();
//
//	ASSERT_EQ(exp, comp);
//}
//
//TEST(PyGraph, NamedMatrixReal) {
//	std::string exp =
//			R"-({"A named matrix":{"colNames":["1","2","3"],"ctype":"Matrix","data":[[1.0,2.0,3.0],[4.0,5.0,6.0],[7.0,8.0,9.0]],"dtype":"Real","ncol":3,"nrow":3,"rowNames":["A","B","C"]}})-";
//	PyGraph gIn;
//	gIn.set(exp);
//
//	Matrix<Real> vec;
//	std::vector<std::string> rowNames;
//	std::vector<std::string> colNames;
//	NamedMatrix<Real> nm = { rowNames, colNames, vec };
//	gIn.get_payload( { }, "A named matrix", nm);
//
//	PyGraph gOut;
//	gOut.add_payload( { }, "A named matrix", nm);
//	std::string comp = gOut.get();
//
//	ASSERT_EQ(exp, comp);
//}
//
//TEST(PyGraph, VectorOfString) {
//	std::string exp = R"-({"var1":["12.0","-35.90","205.72"]})-";
//	PyGraph gIn;
//	gIn.set(exp);
//
//	std::vector<std::string> vec;
//	gIn.get_payload<std::vector<std::string>>( { }, "var1", vec);
//
//	PyGraph gOut;
//	gOut.add_payload( { }, "var1", vec);
//	std::string comp = gOut.get();
//
//	ASSERT_EQ(exp, comp);
//}
//
//TEST(PyGraph, name_payload) {
//	std::string exp = R"-({"var1":"12.0","varZ":"test"})-";
//	PyGraph gIn;
//	gIn.set(exp);
//
//	std::list<std::string> l;
//	gIn.name_payload( { }, l);
//
//	ASSERT_EQ(itString(l), std::string("var1 varZ"));
//}
//
///**
// * This is used in the data output of the Functional model
// */
//TEST(PyGraph, VectorOfVectorOfReal) {
//	std::string exp = R"-({"var1":[[1.0],[1.0,5.0,12.0],[0.0,-42.0]]})-";
//	PyGraph gIn;
//	gIn.set(exp);
//
//	std::vector<std::vector<Real>> temp;
//	gIn.get_payload<std::vector<std::vector<Real>>>( { }, "var1", temp);
//
//	PyGraph gOut;
//	gOut.add_payload( { }, "var1", temp);
//	std::string comp = gOut.get();
//
//	ASSERT_EQ(exp, comp);
//}
//
//TEST(PyGraph, AddSubGraph) {
//	std::string in = R"-({"var": "toto"})-";
//	std::string sub = R"-({"var": "sub-toto"})-";
//	std::string expected = R"-({"subG":{"var":"sub-toto"},"var":"toto"})-";
//
//	PyGraph gIn;
//	gIn.set(in);
//
//	PyGraph subG;
//	subG.set(sub);
//
//	gIn.addSubGraph({}, "subG", subG);
//
//	ASSERT_EQ(gIn.get(), expected);
//}
