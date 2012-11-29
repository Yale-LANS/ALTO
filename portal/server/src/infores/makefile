OBJ = main.o info_resource_base.o info_resource_array.o address_group.o alto_directory.o network_map.o info_resource_entity.o cost_map.o

isort: $(OBJ)
	g++ -o infores $(OBJ)

main.o: main.cpp 
info_resource_base.o: info_resource_base.cpp info_resource_base.h
info_resource_array.o: info_resource_array.cpp info_resource_array.h
address_group.o: address_group.cpp address_group.h
alto_directory.o: alto_directory.cpp alto_directory.h
network_map.o: network_map.cpp network_map.h
info_resource_entity.o: info_resource_entity.cpp info_resource_entity.h
cost_map.o: cost_map.cpp cost_map.h

clean:
	rm *.o infores
