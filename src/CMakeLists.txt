project(reaper)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( reaper reaper reaper.cpp )
target_include_directories( reaper PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( reaper ${CMAKE_SOURCE_DIR}/../ricardian )