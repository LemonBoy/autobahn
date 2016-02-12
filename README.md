# autobahn
Naming things is NP-hard, apparently.

### What
This tiny tool computes the difference between two text files using the same
library that the `git` tool uses and outputs a short summary in a format that
`vim` can easily parse.

### Why
By using the awesome libxdiff fork to do the diffing we're able to use other
algorithms than the Myers one used by `diff`, such as the patience one or the
histogram one. The last two algorithms provide slightly less optimized diffs at
the expense of greatly improved readability.

### How
Just compile the program using the `Makefile` provided and place it in your
`PATH`, that's all you have to do. To get vim use the new harness you just need
to add something like this to your `.vimrc`

```vim
set diffexpr=MyDiff()
function MyDiff()
  let opt = "-1 "
  if &diffopt =~ "iwhite"
    let opt = opt . "-b "
  endif
  silent execute "!autobahn " . opt . v:fname_in . " " . v:fname_new .
  \  " > " . v:fname_out
endfunction
```

As you can see it's just a straight rip-off of the example that can be found in
`diff.txt`, feel free to add more command line switches to the `opt` and/or
change the default algorithm used. When run with no parameters the program will
output a small usage guide.

### Notes

- Not all the features and command line switches that `diff` has are supported.
