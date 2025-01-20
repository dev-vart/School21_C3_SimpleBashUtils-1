    #!/bin/bash

    SUCCESS=0
    FAIL=0
    COUNTER=0
    DIFF_RES=""
    flags=(e i v c l n h o)
    pattern=aa

    declare -a tests=(
    "VAR test_case_cat.txt"
    )

declare -a extra=(
" -e"
"-ivclnho"
"-s"
""

"$pattern -ivclnho test_case_cat.txt"
"$pattern -ivclnh test_case_cat.txt"
"$pattern -ivclno test_case_cat.txt"
"$pattern -ivclho test_case_cat.txt"
"$pattern -ivcnho test_case_cat.txt"
"$pattern -ivlnho test_case_cat.txt"
"$pattern -iclnho test_case_cat.txt"
"$pattern -vclnho test_case_cat.txt"

"$pattern -s no_file.txt"
"$pattern -sh no_file.txt"
"$pattern -sc no_file.txt"
"$pattern -sl no_file.txt"


)

testing()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    ./s21_grep $t > test_s21_grep.log
    grep $t > test_sys_grep.log
    DIFF_RES="$(diff -s test_s21_grep.log test_sys_grep.log)"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "Files test_s21_grep.log and test_sys_grep.log are identical" ]
    then
      (( SUCCESS++ ))
        echo "$FAIL $SUCCESS $COUNTER | success grep $t"
    else
      (( FAIL++ ))
        echo "$FAIL $SUCCESS $COUNTER | fail grep $t"
    fi
    rm test_s21_grep.log test_sys_grep.log
}

if [ "$1" = "valgrind" ]
then 
    testing()
    {
        (( COUNTER++ ))
        t=$(echo $@ | sed "s/VAR/$var/")
        echo "$COUNTER grep $t"
        valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all ./s21_grep $t 2>&1 | grep "ERROR SUMMARY"
    }
fi

#специфические тесты
for i in "${extra[@]}"
do
    var="-"
    testing $i
done

# 1 параметр
for var1 in i v c l n h o
do
    for test in "${tests[@]}"
    do
        var="$pattern -$var1"
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
            var="$pattern -$var1 -$var2"
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
                var="$pattern -$var1 -$var2 -$var3"
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
                    var="$pattern -$var1 -$var2 -$var3 -$var4"
                    testing $test
                done
            done
        done
    done
done

# 5 параметров
for ((i=0; i<${#flags[@]}; i++)) 
do
    for ((j=i+1; j<${#flags[@]}; j++))
    do
        for ((k=j+1; k<${#flags[@]}; k++))
        do
            for ((l=k+1; l<${#flags[@]}; l++))
            do
                for ((p=l+1; p<${#flags[@]}; p++))
                do
                    var1=${flags[i]}
                    var2=${flags[j]}
                    var3=${flags[k]}
                    var4=${flags[l]}
                    var5=${flags[p]}
                
                    for test in "${tests[@]}"
                    do
                        var="$pattern -$var1 -$var2 -$var3 -$var4 -$var5"
                        testing $test
                    done
                done
            done
        done
    done
done

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
echo "ALL: $COUNTER"