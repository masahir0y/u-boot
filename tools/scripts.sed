#s/\(define[[:space:]][[:space:]]*CONFIG_SPL\)$/\1	1/
#s/\(define[[:space:]][[:space:]]*CONFIG_TPL\)$/\1	1/

/define[[:space:]][[:space:]]*CONFIG_\(SPL\|TPL\)[[:space:]][[:space:]]*1/d

#/^CONFIG_\(SPL\|TPL\)_TEXT_BASE/d

#/define[[:space:]][[:space:]]*CONFIG_\(SPL\|TPL\)_TEXT_BASE/d
