addqueue tests/test_dir/cat
addqueue tests/test_dir/world
rmqueue 2
showqueue | awk '{$2=""}{$3=""}1'
