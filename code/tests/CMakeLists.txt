add_custom_target(
        test_archiver
        WORKING_DIRECTORY
        DEPENDS archiver
        COMMAND python3 test.py ${CMAKE_BINARY_DIR}/archiver data
)
