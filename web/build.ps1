mkdir ./release
Compress-Archive -Path ../build/release/bin/*.* -DestinationPath ./release/timmy.zip -Force
Compress-Archive -Path ./data/.jsdos/ -Update ./release/timmy.zip
cp ./data/index.html ./release
cp ./data/title.png ./release
Rename-Item -Path "./release/timmy.zip" -NewName timmy.jsdos
Compress-Archive -Path ./release/*.* -DestinationPath ./release/'Timmy Time Adventure.zip' -Force