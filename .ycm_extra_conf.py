def Settings( **kwargs ):
  return {
    'flags': [
        '-x', 'c',
        '-W' 'Wall', '-Wextra', '-Wpedantic', '-Wdouble-promotion', '-Wstrict-prototypes', '-Wshadow',
        '-std=c11',
        '-I', './include'
    ],
  }
