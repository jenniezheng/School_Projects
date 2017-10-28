javac *.java
while true; do
    java UnsafeMemory
    #wait for changes to script
    inotifywait -qre modify . >> /dev/null;
    javac *.java
done