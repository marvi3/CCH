#include <iostream>
#include <vector>
#include <string>

#include "HTTPServer.h"

#include "Servlets/AlgorithmsAndParameterInformationServlet.h"
#include "Servlets/BoundingBoxServlet.h"
#include "Servlets/JSONServlet.h"
#include "Servlets/FileServlet.h"
#include "Servlets/QueryServletPrototype.h"

#include "../DataStructures/Result.h"
#include "../DataStructures/Parameter.h"
#include "../DataStructures/Geometry/CoordinateTree.h"
#include "../DataStructures/Geometry/Point.h"

#include "../Helpers/Console/CommandLineParser.h"

#include "../../vector_io.h"

#include "../Algorithms/MapServer/HelloWorld.h"


using namespace std;

typedef CoordinateTree<Geometry::GeoMetirc> CoordinateTreeType;


MapServer::HTTPServer server(true);
string test_directory = "Maps/stupferich/";


void usage(char *program) {
    std::cout << "Usage: " << program << " <options>" << std::endl
		      << std::endl
		      << "-d   <directory> -- root directory to be served" << std::endl
		      << "-p   <integer>   -- port number of server (Default: 8080)" << std::endl
		      << "-h               -- print help" << std::endl
              << std::endl
              << "(Try " << program << " -d ../Sites/MapServer)" << std::endl
              << std::endl;
	exit(0);
}

int main(int argc, char **argv) {
	checkAsserts();

	CommandLineParser clp(argc, argv);
	if (clp.isSet("h")) usage(argv[0]);

	const int port(clp.value<int>("p", 8080));
	const string directory(clp.value<string>("d", ""));
	if (directory == "") usage(argv[0]);

	// use "server(true)" in release mode and "server(false)" for debugging!

	server.SetRootDirectory(directory);


        auto first_out = load_vector<unsigned>(test_directory + "first_out");
        auto head = load_vector<unsigned>(test_directory + "head");
        auto latitude = load_vector<float>(test_directory + "latitude");
        auto longitude = load_vector<float>(test_directory + "longitude");

        vector<Geometry::Point> coordinates;

        for (unsigned x = 0; x < first_out.size()-1; ++x) {
            coordinates.push_back(Geometry::Point::LatLong(latitude[x], longitude[x]));
            for (unsigned out_arc = first_out[x]; out_arc < first_out[x+1]; ++out_arc) {

            }
        }


        // Load a list of coordinates
        // Clicking on the map will select the nearest neighbor from this vector


        CoordinateTreeType ct(coordinates);

        BoundingBoxServlet bbs(coordinates);
        server.RegisterServlet(&bbs);

        AlgorithmsAndParameterInformationServlet ais;
        server.RegisterServlet(&ais);

        JSONServlet js;
        ais.registerAlgorithm(js.Handler(), "JSON Texteingabe", js.parameters);
        server.RegisterServlet(&js);

    // Add an algorithm to the serve in 4 steps
    // 1. Create an instance of a class that inherits from class Algorithm in Algorithms/MapServer/Algorithm.h (here HelloWorld).
    // 2. Create a QueryServlet serving your algorithm (First parameter (here "halloWorldServlet") is used via HTTP GET to call the algorithm).
    // 3. Register the QueryServlat with the AlgorithmsAndParameterInformationServlet so that the algorithm can be selected within the browser (Second parameter (here "Hello World!") is displayed to the user as name of the algorithm).
    // 4. Register the QueryServlat with the Server so that the server can invoke execution of the Algorithm if requested.
        HelloWorld halloWorld;
        QueryServletPrototype<CoordinateTreeType> halloWorldServlet("halloWorldServlet", halloWorld, coordinates, ct, first_out, head);
        ais.registerAlgorithm(halloWorldServlet, "Hello World!");
        server.RegisterServlet(&halloWorldServlet);

	server.Start(port);
    do {
        getchar();
    } while(!server.Stop());

	return 0;
}

void load_map(string directory) {

}
