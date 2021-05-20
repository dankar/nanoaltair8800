
set(RELEASE_ARCHIVE "nanoaltair-${VERSION_STRING}.zip")

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${RELEASE_ARCHIVE}
  COMMAND zip -j "${RELEASE_ARCHIVE}"
  "${CMAKE_BINARY_DIR}/firmware/nanoaltair.bin"
  "${CMAKE_BINARY_DIR}/firmware/nanoaltair.elf"
  "${CMAKE_SOURCE_DIR}/README.md"
  "${CMAKE_CURRENT_SOURCE_DIR}/flash_dfu.sh"
  "${CMAKE_BINARY_DIR}/tests/auto_tests_full.log"
  DEPENDS generate_binary full_tests-build)

add_custom_target(create_release DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${RELEASE_ARCHIVE})
