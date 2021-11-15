#!/bin/sh
GRAPH_PATH="/amd.home/algoDaten/praktikum/graph"
ORDER_PATH="/amd.home/algoDaten/praktikum/cch"
FILE_NAME="test_log.txt"
touch ${FILE_NAME}
echo "==========================================================" >> ${FILE_NAME}
echo "" >> ${FILE_NAME}
echo "" >> ${FILE_NAME}

./compile.sh

echo "test1"

./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "travel_time" "elimination_tree" >> ${FILE_NAME}

echo "test2"

./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "travel_time" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "travel_time" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/flowcutter_order" "geo_distance" "basic" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "geo_distance" "elimination_tree" >> ${FILE_NAME}
./main "${GRAPH_PATH}/karlsruhe/" "${ORDER_PATH}/karlsruhe/metis_order" "geo_distance" "basic" >> ${FILE_NAME}

