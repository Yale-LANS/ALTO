MACRO(AddUnitTest target)

	# Get executable path
	GET_TARGET_PROPERTY(testExe ${target} LOCATION)
	
	# Add the test
	ADD_TEST(${target} ${testExe})

ENDMACRO(AddUnitTest)
