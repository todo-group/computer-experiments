file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
foreach(FNAME IN LISTS SRC)
    get_filename_component(TAG ${FNAME} NAME_WE)
    add_executable(${TAG} ${TAG}.c)
    target_include_directories(${TAG} PRIVATE ${INCLUDE})
    target_compile_options(${TAG} PRIVATE ${FLAGS})
    target_compile_features(${TAG} PRIVATE c_std_11)
    target_link_libraries(${TAG} PRIVATE m)
endforeach(FNAME)

target_link_libraries(transfer_matrix PRIVATE ${LINALG_LIBRARIES})