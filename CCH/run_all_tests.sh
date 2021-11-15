#!/bin/sh
GRAPH_PATH="/amd.home/algoDaten/praktikum/graph"
ORDER_PATH="/amd.home/algoDaten/praktikum/cch"
FILE_NAME="test_all_log.txt"
touch ${FILE_NAME}
echo "==========================================================" >> ${FILE_NAME}
echo "" >> ${FILE_NAME}
echo "" >> ${FILE_NAME}

./compile.sh


./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "travel_time" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "travel_time" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "geo_distance" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "geo_distance" "basic" >> ${FILE_NAME}

echo "=========================================================="
echo "karlsruhe not all triangles"

./main "${GRAPH_PATH}/germany/" "${ORDER_PATH}/germany/flowcutter_order" "travel_time" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/germany/" "${ORDER_PATH}/germany/flowcutter_order" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/germany/" "${ORDER_PATH}/germany/flowcutter_order" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/germany/" "${ORDER_PATH}/germany/flowcutter_order" "geo_distance" "basic" >> ${FILE_NAME}

echo "=========================================================="
echo "germany not all triangles"

./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/flowcutter_order" "travel_time" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/flowcutter_order" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/metis_order" "travel_time" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/metis_order" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/kahip_order" "travel_time" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/kahip_order" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/flowcutter_order" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/flowcutter_order" "geo_distance" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/metis_order" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/metis_order" "geo_distance" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/kahip_order" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/kahip_order" "geo_distance" "basic" >> ${FILE_NAME}

echo "=========================================================="
echo "europe not all triangles"

./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.gr" "travel_time" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.gr" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.kahip.order" "travel_time" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.kahip.order" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.order" "travel_time" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.order" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.gr" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.gr" "geo_distance" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.kahip.order" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.kahip.order" "geo_distance" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.order" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.order" "geo_distance" "basic" >> ${FILE_NAME}

echo "=========================================================="
echo "TheFrozenSea not all triangles"
echo "=========================================================="
echo "not all triangles done"




./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "travel_time" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "travel_time" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "travel_time" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "travel_time" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "geo_distance" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "geo_distance" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "geo_distance" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "geo_distance" "basic" "all_triangles" >> ${FILE_NAME}

echo "=========================================================="
echo "not all triangles"

./main "${GRAPH_PATH}/germany/" "${ORDER_PATH}/germany/flowcutter_order" "travel_time" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/germany/" "${ORDER_PATH}/germany/flowcutter_order" "travel_time" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/germany/" "${ORDER_PATH}/germany/flowcutter_order" "geo_distance" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/germany/" "${ORDER_PATH}/germany/flowcutter_order" "geo_distance" "basic" "all_triangles" >> ${FILE_NAME}

./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/flowcutter_order" "travel_time" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/flowcutter_order" "travel_time" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/metis_order" "travel_time" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/metis_order" "travel_time" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/kahip_order" "travel_time" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/kahip_order" "travel_time" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/flowcutter_order" "geo_distance" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/flowcutter_order" "geo_distance" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/metis_order" "geo_distance" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/metis_order" "geo_distance" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/kahip_order" "geo_distance" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/europe/" "${ORDER_PATH}/europe/kahip_order" "geo_distance" "basic" "all_triangles" >> ${FILE_NAME}

./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.gr" "travel_time" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.gr" "travel_time" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.kahip.order" "travel_time" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.kahip.order" "travel_time" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.order" "travel_time" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.order" "travel_time" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.gr" "geo_distance" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.gr" "geo_distance" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.kahip.order" "geo_distance" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.kahip.order" "geo_distance" "basic" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.order" "geo_distance" "elimination_tree" "all_triangles" >> ${FILE_NAME}
./main "${GRAPH_PATH}/TheFrozenSea/" "${ORDER_PATH}/TheFrozenSea/TheFrozenSea.order" "geo_distance" "basic" "all_triangles" >> ${FILE_NAME}

