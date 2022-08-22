cmake_minimum_required(VERSION 3.14)

enable_language(Fortran)

project(compexp C CXX Fortran)

find_package(BLAS)
find_package(LAPACK)

if (LAPACK_FOUND AND BLAS_FOUND)
    set(LINALG_LIBRARIES ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES})
else()
    message(FATAL_ERROR "NO LAPACK/BLAS FOUND")
endif()


set(FLAGS -O3 -flto -Wall -Wextra -Wcast-qual -Winit-self -Wlogical-op
    -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wfloat-equal -Wno-unused-result)
set(FFLAGS -O3 -flto -Wall -Wextra -Wmissing-include-dirs -Wshadow)

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(basics)
add_subdirectory(eigenvalue_problem)
add_subdirectory(hpc)
add_subdirectory(linear_regression)
add_subdirectory(linear_system)
add_subdirectory(matrix)
add_subdirectory(monte_carlo)
add_subdirectory(optimization)
add_subdirectory(svd)
add_subdirectory(svd_generate)