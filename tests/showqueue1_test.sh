addqueue tests/test_dir/cat
addqueue tests/test_dir/world
showqueue | awk '{$2=""}{$3=""}1'
