cd release/ && make runUtest && cd ..
MC_DETERMINISTIC=T release/utest/runUtest --gtest_output=xml:report.xml
