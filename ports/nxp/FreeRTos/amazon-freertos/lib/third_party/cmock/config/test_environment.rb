# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

# Setup our load path:
[
  './lib',
  './vendor/behaviors/lib',
  './vendor/hardmock/lib',
  './vendor/unity/auto/',
  './test/system/'
].each do |dir|
  $LOAD_PATH.unshift( File.join( File.expand_path(File.dirname(__FILE__) + "/../"), dir) )
end
