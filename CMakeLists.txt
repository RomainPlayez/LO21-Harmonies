cmake_minimum_required(VERSION 3.16)
project(Harmonies VERSION 0.1 LANGUAGES CXX)

include(CTest)

# Make CTest output easier to read by default.
set(CTEST_OUTPUT_ON_FAILURE ON)
set(CTEST_PROGRESS_OUTPUT ON)

# --- Paramètres Qt 6 obligatoires ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- Paramètres C++ ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Optimisation du build ---
option(HARMONIES_UNITY_BUILD "Enable unity build for faster compilation" OFF)
if(HARMONIES_UNITY_BUILD)
	set(CMAKE_UNITY_BUILD ON)
	set(CMAKE_UNITY_BUILD_BATCH_SIZE 4)
endif()
set(CMAKE_JOB_POOLS "default=4")
set(CMAKE_JOB_POOL_LINK default)

option(HARMONIES_ENABLE_ASAN "Enable Address/Leak sanitizers in Debug builds" OFF)
if(HARMONIES_ENABLE_ASAN)
	if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
		add_compile_options(
			$<$<CONFIG:Debug>:-fsanitize=address,leak>
			$<$<CONFIG:Debug>:-fno-omit-frame-pointer>
		)
		add_link_options($<$<CONFIG:Debug>:-fsanitize=address,leak>)
		message(STATUS "AddressSanitizer/LeakSanitizer enabled for Debug builds")
	else()
		message(WARNING "HARMONIES_ENABLE_ASAN is ON, but compiler '${CMAKE_CXX_COMPILER_ID}' is not supported")
	endif()
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/compile_commands.json")
	file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json"
		"${CMAKE_SOURCE_DIR}/compile_commands.json"
		SYMBOLIC)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE HARMONIES_CORE_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp")

add_library(${PROJECT_NAME}_core STATIC)

target_sources(${PROJECT_NAME}_core
	PRIVATE
		${HARMONIES_CORE_CPP_FILES}
)

target_compile_features(${PROJECT_NAME}_core PUBLIC cxx_std_17)

target_compile_definitions(${PROJECT_NAME}_core
	PRIVATE
		HARMONIES_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

# On indique au Core où chercher ses fichiers .h
target_include_directories(${PROJECT_NAME}_core
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# ==============================================================================
# 2. L'EXÉCUTABLE FINAL (L'interface Qt + Fichiers de lancement)
# ==============================================================================
file(GLOB_RECURSE HARMONIES_GUI_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/*.cpp")
file(GLOB_RECURSE HARMONIES_GUI_H_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/gui/*.h")

# Fichiers à la racine
set(ROOT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp")
set(ROOT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h")

set(GUI_FORMS "${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui")
set(RESSOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ressources.qrc")

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		${HARMONIES_GUI_CPP_FILES}
		${HARMONIES_GUI_H_FILES} 
		${ROOT_SOURCES}
		${ROOT_HEADERS}         
		${GUI_FORMS}
		${RESSOURCES}
)

target_include_directories(${PROJECT_NAME}
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		${PROJECT_NAME}_core
		Qt6::Widgets
)


if(BUILD_TESTING)
	find_package(Catch2 3 QUIET)
	if(Catch2_FOUND)
		file(GLOB_RECURSE HARMONIES_TEST_FILES
			"${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
		)

		foreach(test_file IN LISTS HARMONIES_TEST_FILES)
			get_filename_component(test_stem "${test_file}" NAME_WE)
			get_filename_component(test_dir "${test_file}" DIRECTORY)
			file(RELATIVE_PATH test_rel_dir "${CMAKE_CURRENT_SOURCE_DIR}/tests" "${test_dir}")
			if(test_rel_dir STREQUAL "")
				set(test_rel_dir "root")
			endif()

			string(MAKE_C_IDENTIFIER "${test_rel_dir}" group_id)
			string(MAKE_C_IDENTIFIER "${test_rel_dir}_${test_stem}" test_id)
			string(REPLACE "/" "." group_label "${test_rel_dir}")
			string(REPLACE "\\" "." group_label "${group_label}")

			set(test_target "${PROJECT_NAME}_test_${test_id}")
			set(test_name "${PROJECT_NAME}.${group_label}.${test_stem}")

			add_executable(${test_target})
			target_sources(${test_target}
				PRIVATE
					"${test_file}"
			)

			target_link_libraries(${test_target}
				PRIVATE
					${PROJECT_NAME}_core
					Catch2::Catch2WithMain
			)

			add_test(NAME "${test_name}" COMMAND ${test_target}
				--reporter compact
				--durations yes
			)
			set_tests_properties("${test_name}"
				PROPERTIES LABELS "${group_id}"
			)
		endforeach()
	else()
		message(STATUS "Catch2 not found. Run 'conan install . -s build_type=Release' before configuring tests.")
	endif()
endif()