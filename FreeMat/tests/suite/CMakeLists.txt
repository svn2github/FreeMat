
enable_testing()

ADD_TEST(test_assign10 ${FreeMat_Loc} "-nogui" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/suite" "-f" "~test_assign10")
ADD_TEST(test_assign11 ${FreeMat_Loc} "-nogui" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/suite" "-f" "~test_assign11")
ADD_TEST(test_assign12 ${FreeMat_Loc} "-nogui" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/suite" "-f" "~test_assign12")
ADD_TEST(test_assign13 ${FreeMat_Loc} "-nogui" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/suite" "-f" "~test_assign13")
