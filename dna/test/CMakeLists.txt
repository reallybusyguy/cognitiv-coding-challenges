

set(CLASSES
		../Chromosome_Comparer.cpp
		../Chromosome_Comparison.cpp
		../DNA_Stream.cpp
		../Person.cpp
		../String_Comparer.cpp
		../Transformation.cpp
)

set(TESTS
		fake_stream.cpp
		fake_stream_test.cpp
		sequence_buffer_test.cpp
		Chromosome_Comparer_test.cpp
		Person_test.cpp
		String_Comparer_test.cpp
)

add_executable(dna_test ${CLASSES} ${TESTS} main.cpp)
target_link_libraries(dna_test cogdna)
