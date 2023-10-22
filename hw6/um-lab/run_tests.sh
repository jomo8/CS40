#! /bin/sh
testFiles=$(ls $2 | grep '\.um$')
for testFile in $testFiles 
do
        testName=$(echo $testFile | sed -E 's/(.*).um/\1/')
        dumpFile="${testName}.dump"
        tmpFile="${testName}.1"
        touch $dumpFile
        if [ -f $tmpFile ]; then
                echo "$testName calling umdump "
                umdump $testFile > $dumpFile
        else
                echo "$testName has no output!"
        fi
        
done
