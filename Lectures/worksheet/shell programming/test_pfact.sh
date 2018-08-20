for arg in "$@"  # or $* will work here 
do
  ./pfact $arg
done

# Alternative:
# for i 
# do
# 	./pfact $i
# done
