enable_testing()
find_package(PythonInterp)

file(GLOB_RECURSE PY_FILES ${PROJECT_SOURCE_DIR}/python/*.py)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/.pytimestamp
                   COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel --dist-dir=${PROJECT_BINARY_DIR} --quiet --universal
                   COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_BINARY_DIR}/.pytimestamp
                   DEPENDS dsblock-lib ${PY_FILES}
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/python)

add_custom_target(dsblock-dist ALL
                  DEPENDS ${PROJECT_BINARY_DIR}/.pytimestamp)

add_dependencies(dsblock-dist dsblock-lib)

add_test(NAME PyDSBlockUintTests
         COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/python/setup.py test
         WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
