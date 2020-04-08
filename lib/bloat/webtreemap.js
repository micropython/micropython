/* Modified version for MicroPython from https://github.com/evmar/webtreemap */
/* @license
 *  Copyright 2013 Google Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http:www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

;(function(root, factory) {
  if (typeof define === 'function' && define.amd)
    define([], factory);
  else if (typeof module === 'object' && module.exports)
    module.exports = factory();
  else
    root.appendTreemap = factory();
}(this, function() {
// Size of border around nodes.
// We could support arbitrary borders using getComputedStyle(), but I am
// skeptical the extra complexity (and performance hit) is worth it.
var kBorderWidth = 1;

// Padding around contents.
// TODO: do this with a nested div to allow it to be CSS-styleable.
var kPadding = 4;

// x/y ratio to aim for -- wider rectangles are better for text display
var kAspectRatio = 1.2;

var focused = null;

function focus(tree) {
  focused = tree;

  // Hide all visible siblings of all our ancestors by lowering them.
  var level = 0;
  var root = tree;
  while (root.parent) {
    root = root.parent;
    level += 1;
    for (var i = 0, sibling; sibling = root.children[i]; ++i) {
      if (sibling.dom)
        sibling.dom.style.zIndex = 0;
    }
  }
  var width = root.dom.offsetWidth;
  var height = root.dom.offsetHeight;
  // Unhide (raise) and maximize us and our ancestors.
  for (var t = tree; t.parent; t = t.parent) {
    // Shift off by border so we don't get nested borders.
    // TODO: actually make nested borders work (need to adjust width/height).
    position(t.dom, -kBorderWidth, -kBorderWidth, width, height);
    t.dom.style.zIndex = 1;
  }
  // And layout into the topmost box.
  layout(tree, level, width, height);
}

function makeDom(tree, level) {
  var dom = document.createElement('div');
  dom.style.zIndex = 1;
  dom.className = 'webtreemap-node webtreemap-level' + Math.min(level, 4);
  if (tree.data['symbol']) {
    dom.className += (' webtreemap-symbol-' +
	tree.data['symbol'].replace(' ', '_'));
  }
  if (tree.data['dominant_symbol']) {
    dom.className += (' webtreemap-symbol-' +
	tree.data['dominant_symbol'].replace(' ', '_'));
    dom.className += (' webtreemap-aggregate');
  }

  for(key in tree.data){
    if(key != 'area'){
      dom.setAttribute('data-' + key, tree.data[key]);
    }
  }

  dom.onmousedown = function(e) {
    if (e.button == 0) {
      if (focused && tree == focused && focused.parent) {
        focus(focused.parent);
      } else {
        focus(tree);
      }
    }
    e.stopPropagation();
    return true;
  };

  var caption = document.createElement('div');
  caption.className = 'webtreemap-caption';
  caption.innerHTML = tree.name;
  dom.appendChild(caption);
  dom.title = tree.name;

  tree.dom = dom;
  return dom;
}

function position(dom, x, y, width, height) {
  // CSS width/height does not include border.
  width -= kBorderWidth*2;
  height -= kBorderWidth*2;

  dom.style.left   = x + 'px';
  dom.style.top    = y + 'px';
  dom.style.width  = Math.max(width, 0) + 'px';
  dom.style.height = Math.max(height, 0) + 'px';
}

// Given a list of rectangles |nodes|, the 1-d space available
// |space|, and a starting rectangle index |start|, compute an span of
// rectangles that optimizes a pleasant aspect ratio.
//
// Returns [end, sum], where end is one past the last rectangle and sum is the
// 2-d sum of the rectangles' areas.
function selectSpan(nodes, space, start) {
  // Add rectangle one by one, stopping when aspect ratios begin to go
  // bad.  Result is [start,end) covering the best run for this span.
  // http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.36.6685
  var node = nodes[start];
  var rmin = node.data['area'];  // Smallest seen child so far.
  var rmax = rmin;                // Largest child.
  var rsum = 0;                   // Sum of children in this span.
  var last_score = 0;             // Best score yet found.
  for (var end = start; node = nodes[end]; ++end) {
    var size = node.data['area'];
    if (size < rmin)
      rmin = size;
    if (size > rmax)
      rmax = size;
    rsum += size;

    // This formula is from the paper, but you can easily prove to
    // yourself it's taking the larger of the x/y aspect ratio or the
    // y/x aspect ratio.  The additional magic fudge constant of kAspectRatio
    // lets us prefer wider rectangles to taller ones.
    var score = Math.max(space*space*rmax / (rsum*rsum),
                         kAspectRatio*rsum*rsum / (space*space*rmin));
    if (last_score && score > last_score) {
      rsum -= size;  // Undo size addition from just above.
      break;
    }
    last_score = score;
  }
  return [end, rsum];
}

function layout(tree, level, width, height) {
  if (!('children' in tree))
    return;

  var total = tree.data['area'];

  // XXX why do I need an extra -1/-2 here for width/height to look right?
  var x1 = 0, y1 = 0, x2 = width - 1, y2 = height - 2;
  x1 += kPadding; y1 += kPadding;
  x2 -= kPadding; y2 -= kPadding;
  y1 += 14;  // XXX get first child height for caption spacing

  var pixels_to_units = Math.sqrt(total / ((x2 - x1) * (y2 - y1)));

  for (var start = 0, child; child = tree.children[start]; ++start) {
    if (x2 - x1 < 60 || y2 - y1 < 40) {
      if (child.dom) {
        child.dom.style.zIndex = 0;
        position(child.dom, -2, -2, 0, 0);
      }
      continue;
    }

    // Dynamically decide whether to split in x or y based on aspect ratio.
    var ysplit = ((y2 - y1) / (x2 - x1)) > kAspectRatio;

    var space;  // Space available along layout axis.
    if (ysplit)
      space = (y2 - y1) * pixels_to_units;
    else
      space = (x2 - x1) * pixels_to_units;

    var span = selectSpan(tree.children, space, start);
    var end = span[0], rsum = span[1];

    // Now that we've selected a span, lay out rectangles [start,end) in our
    // available space.
    var x = x1, y = y1;
    for (var i = start; i < end; ++i) {
      child = tree.children[i];
      if (!child.dom) {
        child.parent = tree;
        child.dom = makeDom(child, level + 1);
        tree.dom.appendChild(child.dom);
      } else {
        child.dom.style.zIndex = 1;
      }
      var size = child.data['area'];
      var frac = size / rsum;
      if (ysplit) {
        width = rsum / space;
        height = size / width;
      } else {
        height = rsum / space;
        width = size / height;
      }
      width /= pixels_to_units;
      height /= pixels_to_units;
      width = Math.round(width);
      height = Math.round(height);
      position(child.dom, x, y, width, height);
      if ('children' in child) {
        layout(child, level + 1, width, height);
      }
      if (ysplit)
        y += height;
      else
        x += width;
    }

    // Shrink our available space based on the amount we used.
    if (ysplit)
      x1 += Math.round((rsum / space) / pixels_to_units);
    else
      y1 += Math.round((rsum / space) / pixels_to_units);

    // end points one past where we ended, which is where we want to
    // begin the next iteration, but subtract one to balance the ++ in
    // the loop.
    start = end - 1;
  }
}

// The algorithm does best at laying out items from largest to smallest.
// Recursively sort the tree to ensure this.
function treeSort(tree) {
  tree.children.sort(function (a, b) {
    return b.data['area'] - a.data['area'];
  });
  for (var i = 0; i < tree.children.length; ++i) {
    var child = tree.children[i];
    if ('children' in child) {
      treeSort(child);
    }
  }
}

function appendTreemap(dom, data, options) {
  var style = getComputedStyle(dom, null);
  var width = parseInt(style.width);
  var height = parseInt(style.height);
  if (options === undefined || options.sort !== false) {
    treeSort(data);
  }
  if (!data.dom)
    makeDom(data, 0);
  dom.appendChild(data.dom);
  position(data.dom, 0, 0, width, height);
  layout(data, 0, width, height);
}

return appendTreemap;
}));
