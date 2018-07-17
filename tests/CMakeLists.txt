project(Annwvyn)

file(GLOB TestCode src/* include/*)

#we are deactivating the properties about PCH because they prevent the code from compilinb right now

if(MSVC)
    #    file(GLOB TestPCH pch/*)
endif()

add_executable(AnnwvynUnitTest ${TestCode})
target_link_libraries(AnnwvynUnitTest
    Annwvyn
    ${OGRE_LIBRARIES}
    ${OGRE_HlmsPbs_LIBRARIES}
    ${OGRE_HlmsUnlit_LIBRARIES}
    ${OGRE_Overlay_LIBRARIES}
)

target_include_directories(AnnwvynUnitTest PRIVATE include/
    )

cotire(AnnwvynUnitTest)

enable_testing()
add_test(NAME Run COMMAND AnnwvynUnitTest)
add_test(NAME ListTests COMMAND AnnwvynUnitTest --list-tests)
set_tests_properties(ListTests PROPERTIES PASS_REGULAR_EXPRESSION "[0-9]+ test cases")
add_test(NAME ListTags COMMAND AnnwvynUnitTest --list-tags)
set_tests_properties(ListTags PROPERTIES PASS_REGULAR_EXPRESSION "[0-9]+ tags")
