def fib(x)
  if x < 3
    1
  else
    fib(x-1) + fib(x-2)
  end
end

print fib(40)
print "Hello World"
