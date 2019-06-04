/**
 * @license MIT
 */

const browserify = require('browserify');
const buffer = require('vinyl-buffer');
const fs = require('fs-extra');
const gulp = require('gulp');
const path = require('path');
const merge = require('merge-stream');
const mocha = require('gulp-mocha');
const sorcery = require('sorcery');
const source = require('vinyl-source-stream');
const sourcemaps = require('gulp-sourcemaps');
const ts = require('gulp-typescript');
const util = require('gulp-util');

const buildDir = process.env.BUILD_DIR || 'build';
const tsProject = ts.createProject('src/tsconfig.json');
let srcDir = './src';
let outDir = './lib';

const addons = fs.readdirSync(`${__dirname}/src/addons`);

const TEST_PATHS = [
  `${outDir}/*test.js`,
  `${outDir}/**/*test.js`,
  `${outDir}/*integration.js`,
  `${outDir}/**/*integration.js`
];

// Under some environments like TravisCI, this comes out at absolute which can
// break the build. This ensures that the outDir is absolute.
if (path.normalize(outDir).indexOf(__dirname) !== 0) {
  outDir = `${__dirname}/${path.normalize(outDir)}`;
}

gulp.task('css', function() {
  return gulp.src(`${srcDir}/**/*.css`).pipe(gulp.dest(outDir));
});

gulp.task('css-build', function() {
  return gulp.src(`${srcDir}/**/*.css`).pipe(gulp.dest(buildDir));
});

gulp.task('watch-css', function() {
  return gulp.watch(`${srcDir}/**/*.css`, ['css']);
});

/**
 * Bundle JavaScript files produced by the `tsc` task, into a single file named `xterm.js` with
 * Browserify.
 */
gulp.task('browserify', function() {
  // Ensure that the build directory exists
  fs.ensureDirSync(buildDir);

  let browserifyOptions = {
    basedir: buildDir,
    debug: true,
    entries: [`${outDir}/xterm.js`],
    standalone: 'Terminal',
    cache: {},
    packageCache: {}
  };
  let bundleStream = browserify(browserifyOptions)
        .bundle()
        .pipe(source(`xterm.js`))
        .pipe(buffer())
        .pipe(sourcemaps.init({loadMaps: true, sourceRoot: '..'}))
        .pipe(sourcemaps.write('./'))
        .pipe(gulp.dest(buildDir));

  // Copy stylesheets from ${outDir}/ to ${buildDir}/
  let copyStylesheets = gulp.src(`${outDir}/**/*.css`).pipe(gulp.dest(buildDir));

  return merge(bundleStream, copyStylesheets);
});

gulp.task('browserify-addons', function() {
  const bundles = addons.map((addon) => {
    const addonOptions = {
      basedir: `${buildDir}/addons/${addon}`,
      debug: true,
      entries: [`${outDir}/addons/${addon}/${addon}.js`],
      standalone: addon,
      cache: {},
      packageCache: {}
    };

    const addonBundle = browserify(addonOptions)
      .external(path.join(outDir, 'Terminal.js'))
      .bundle()
      .pipe(source(`./addons/${addon}/${addon}.js`))
      .pipe(buffer())
      .pipe(sourcemaps.init({loadMaps: true, sourceRoot: ''}))
      .pipe(sourcemaps.write('./'))
      .pipe(gulp.dest(buildDir));

    return addonBundle;
  });

  return merge(...bundles);
});

gulp.task('mocha', function () {
  return gulp.src(TEST_PATHS, {read: false})
      .pipe(mocha())
      .once('error', () => process.exit(1));
});

/**
 * Run single test suite (file) by file name (without file extension). Example of the command:
 * gulp mocha-suite --test InputHandler.test
 */
gulp.task('mocha-suite', [], function () {
  let testName = util.env.test;
  util.log("Run test by Name: " + testName);
  return gulp.src([`${outDir}/${testName}.js`, `${outDir}/**/${testName}.js`], {read: false})
         .pipe(mocha())
         .once('error', () => process.exit(1));
});

/**
 * Use `sorcery` to resolve the source map chain and point back to the TypeScript files.
 * (Without this task the source maps produced for the JavaScript bundle points into the
 * compiled JavaScript files in ${outDir}/).
 */
gulp.task('sorcery', ['browserify'], function () {
  let chain = sorcery.loadSync(`${buildDir}/xterm.js`);
  chain.apply();
  chain.writeSync();
});

gulp.task('sorcery-addons', ['browserify-addons'], function () {
  addons.forEach((addon) => {
    const chain = sorcery.loadSync(`${buildDir}/addons/${addon}/${addon}.js`);
    chain.apply();
    chain.writeSync();
  })
});

gulp.task('build', ['css', 'css-build', 'sorcery', 'sorcery-addons']);
gulp.task('test', ['mocha']);
gulp.task('default', ['build']);
