#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""
flags=(b e n s t v)

declare -a tests=(
"VAR test_case_cat.txt"
)

declare -a extra=(
"-s test_1_cat.txt"
"-benstv test_1_cat.txt"
"--squeeze-blank --number-nonblank --number test_1_cat.txt"
"--number test_1_cat.txt"
"-t test_3_cat.txt"
"-n test_2_cat.txt"
"no_file.txt"
"-n -b test_1_cat.txt"
"-s -n -e test_4_cat.txt"
"test_1_cat.txt -n"
"-n test_1_cat.txt"
"-n test_1_cat.txt test_2_cat.txt"
"-v test_5_cat.txt"
"-benst test_1_cat.txt"
"-bensv test_1_cat.txt"
"-bentv test_1_cat.txt"
"-bestv test_1_cat.txt"
"-bnstv test_1_cat.txt"
"-enstv test_1_cat.txt"
"-benstv 1.txt 2.txt"
"-ET 1.txt 2.txt"
)

testing()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    ./s21_cat $t > test_s21_cat.log
    cat $t > test_sys_cat.log
    DIFF_RES="$(diff -s test_s21_cat.log test_sys_cat.log)"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "Files test_s21_cat.log and test_sys_cat.log are identical" ]
    then
      (( SUCCESS++ ))
        echo "$FAIL $SUCCESS $COUNTER | success cat $t"
    else
      (( FAIL++ ))
        echo "$FAIL $SUCCESS $COUNTER | fail cat $t"
    fi
    rm test_s21_cat.log test_sys_cat.log
}

if [ "$1" = "valgrind" ]
then 
    testing()
    {
        t=$(echo $@ | sed "s/VAR/$var/")
        valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all ./s21_cat $t 2>&1 | grep "ERROR SUMMARY"
    }
fi


#специфические тесты
for i in "${extra[@]}"
do
    var="-"
    testing $i
done

#1 параметр
for var1 in b e n s t v
do
    for test in "${tests[@]}"
    do
        var="-$var1"
        testing $test
    done
done

# 2 параметра
for ((i=0; i<${#flags[@]}; i++))
do
    for ((j=i+1; j<${#flags[@]}; j++))
    do
        var1=${flags[i]}
        var2=${flags[j]}
        
        for test in "${tests[@]}"
        do
            var="-$var1 -$var2"
            testing $test
        done
    done
done

# 3 параметра
for ((i=0; i<${#flags[@]}; i++)) 
do
    for ((j=i+1; j<${#flags[@]}; j++))
    do
        for ((k=j+1; k<${#flags[@]}; k++))
        do
            var1=${flags[i]}
            var2=${flags[j]}
            var3=${flags[k]}
        
            for test in "${tests[@]}"
            do
                var="-$var1 -$var2 -$var3"
                testing $test
            done
        done
    done
done

# 4 параметра
for ((i=0; i<${#flags[@]}; i++)) 
do
    for ((j=i+1; j<${#flags[@]}; j++))
    do
        for ((k=j+1; k<${#flags[@]}; k++))
        do
            for ((l=k+1; l<${#flags[@]}; l++))
            do
                var1=${flags[i]}
                var2=${flags[j]}
                var3=${flags[k]}
                var4=${flags[l]}
            
                for test in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3 -$var4"
                    testing $test
                done
            done
        done
    done
done

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
echo "ALL: $COUNTER"
