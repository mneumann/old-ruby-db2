

export: clean 
	mkdir -p work  
	(cd work; cvs export -D now ruby/ruby-db2)
	(cd work/ruby/ruby-db2; rm -rf utils Makefile)
clean:
	rm -rf work  

pack:
	mkdir -p work/pack  
	( cd work/pack; cvs export -r Release-$(TAG_MAJOR)_$(TAG_MINOR) ruby/ruby-db2; \
	  cd ruby/ruby-db2; rm -rf utils Makefile;                                     \
	  cd ..; tar -cvzf ruby-db2-$(TAG_MAJOR).$(TAG_MINOR).tar.gz ruby-db2 )

