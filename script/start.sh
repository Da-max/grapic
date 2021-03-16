echo "Nom de l’application à start"
read application
make "$application" && ./bin/"$application"