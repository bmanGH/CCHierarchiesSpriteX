/*
 * Hierarchies Sprite Animation Exporter
 *
 * author: bman <zx123xz321hm3@hotmail.com>
 *
 * version 0.1   12/22/2011
 * version 0.2   12/29/2011
 * version 0.3   12/31/2011
 * version 0.4   1/13/2012   support color transform
 * version 0.5   1/16/2012   support alpha transform and cook guide layer animation to key frames
 * version 0.6   2/3/2012    use item index to identification element instead of name; add library items' left and bottom properties
 * version 0.7   2/10/2012   add avatar, dyeing, event functions
 * version 0.8   2/27/2012   add single texture batch mode function
 * version 0.9   2/28/2012   add CCHierarchiesSpriteSheetCache, CCHierarchiesAnimationCache
 * version 0.9.1 3/15/2012   simplify work if no avater and dyeing
 * version 0.9.2 3/22/2012   fix copy string without '\0' bug
 * version 0.10  3/23/2012   add multi-thread animation in batch mode
 * version 0.10.1   3/26/2012   export .sprites file and items image automatic
 * version 0.10.2   3/26/2012   chinese localization
 * version 0.10.3   3/26/2012   support set .sprites, .anims file, set batch after init
 * version 0.11     3/28/2012   support multi texture packer in batch mode
 * version 0.11.1   4/17/2012   support retina sprites file export
 * version 0.11.2   5/21/2012   hack export .sprites precision bug
 * version 0.12     6/5/2012    replace libxml2 with rapidxml
 * version 0.12.1   6/5/2012    add setAvatarTag and setAvatarTags to CCHierarchiesSprite class
 * version 0.12.2   6/6/2012    fix no-display bug in release build configure
 * version 0.13     11/6/2012   separate version between .hanims file format and runtime code
 * version 0.13.1   31/8/2012   remove retina .sprites export
 * version 0.13.2   4/9/2012    export POT texture
 * version 0.14     16/4/2013   remove animation loop and color dyeing
 * version 0.14.1   6/7/2013    add trace log
 * version 0.20     8/9/2013    add support nesting movieclip and graphic
 * version 0.25     8/19/2013   add filter support
 * version 0.26     9/18/2013   separate "convert guided frames to key frames" and "convert shape to graphic" to HierarchiesSpriteAnimationPreprocess.jsfl
 * version 0.27     9/22/2013   add support symbol instance's looping options
 * version 0.28     9/27/2013   merge sub sprites to one animation file, move item's left and bottom property to animation file, rename .sprites to .hsheet
 * version 0.29     9/30/2013   use abbreviation xml tag
 * version 0.29.1   10/20/2013  support movie clip symbol frame offset inherit without loop
 * version 0.29.2   11/28/2013  fix save .hanims file fail which include folder uri on windows platform
 */


//------------ Constant --------------

var EXPORTER_VERSION = "0.29";									// format verstion
var ANIM_TAGS_LAYER_NAME = "anim_tags";							// animation data layer name
var SPACE_SHIFT_STEP = 4;										// export xml's space shift step
var EVENT_TAGS_LAYER_NAME = "event_tags";						// event data layer name
var ITEMS_ATLAS_MAX_WIDTH = 2048;								// export sprite sheet image width
var ITEMS_SPACING = 4;											// export sprite sheet iamge spacing
var IGNORE_ITEM_NAME_PREFIX = "IGNORE__";						// ignore item name prefix

var SKIP_EXPORT_BITMAP = true;									// don't export bitmap if 'true'
var SKIP_EXPORT_FILTER = true;									// don't export filter if 'true'
var EXPORT_PACK_IMAGE_SHEET = true;							// export packed image sheet if 'true'

//------------------------------------


//------------ Global variable -------

var exportSprites = {};							// export sprite data, key is sprite name
var exportItems = [];							// export item data

//------------------------------------


//------------ Export data structure -------------

ExportElement = function () {
	this.symbolIndex = 0;
	this.x = 0;
	this.y = 0;
	this.anchorX = 0;
	this.anchorY = 0;
	this.scaleX = 1;
	this.scaleY = 1;
	// this.rotation = 0; //!!! rotation is same as scale + skew in flash
	this.skewX = 0;
	this.skewY = 0;
	// this.depth = 0;
	this.color_alpha_percent = 1;
	this.color_alpha_amount = 0;
	this.color_red_percent = 1;
	this.color_red_amount = 0;
	this.color_green_percent = 1;
	this.color_green_amount = 0;
	this.color_blue_percent = 1;
	this.color_blue_amount = 0;
	// if symbol is a socket than looping should be 'loop', 'play once' or 'single frame', other should be 'none'
	this.loopMode = "none";
	this.frameOffset = 0;
	this.frameOffsetInherited = false; // NOT export
	this.filters = [];
}
ExportElement.prototype.toXML = function (shift) {
	var spaces = shiftSpaces(shift);
	var xml = spaces + '<E' 
			+ ' si="' + this.symbolIndex + '"'
			+ ' x="' + this.x + '"'
			+ ' y="' + this.y + '"'
			+ ' ax="' + this.anchorX + '"'
			+ ' ay="' + this.anchorY + '"'
			+ ' sx="' + this.scaleX + '"'
			+ ' sy="' + this.scaleY + '"'
			// + ' rot="' + this.rotation + '"'
			+ ' kx="' + this.skewX + '"'
			+ ' ky="' + this.skewY + '"'
			// + ' dep="' + this.depth + '"'
			+ ' ap="' + this.color_alpha_percent + '"'
			+ ' aa="' + this.color_alpha_amount + '"'
			+ ' rp="' + this.color_red_percent + '"'
			+ ' ra="' + this.color_red_amount + '"'
			+ ' gp="' + this.color_green_percent + '"'
			+ ' ga="' + this.color_green_amount + '"'
			+ ' bp="' + this.color_blue_percent + '"'
			+ ' ba="' + this.color_blue_amount + '"'
			+ ' lo="' + this.loopMode + '"'
			+ ' fo="' + this.frameOffset + '"'
			+ ' >\n';

	for (var l = 0; l < this.filters.length; l++) {
		xml += this.filters[l].toXML(shift + SPACE_SHIFT_STEP);
	}

	xml += spaces + '</E>\n';
	return xml;
}

ExportKeyFrame = function () {
	this.id = 0;
	this.duration = 0;
	this.elements = [];
	this.isMotion = false;
}
ExportKeyFrame.prototype.toXML = function (shift) {
	var spaces = shiftSpaces(shift);
	var xml = spaces + '<F' 
			+ ' id="' + this.id + '"'
			+ ' dur="' + this.duration + '"'
			+ ' mot="' + this.isMotion + '"'
			+ ' >\n';

	// // resort element by depth
	// this.elements.sort(function (a, b) {
	// 	return a.depth - b.depth;
	// });

	for (var i = 0; i < this.elements.length; i++) {
		xml += this.elements[i].toXML(shift + SPACE_SHIFT_STEP);
	}

	xml += spaces + '</F>\n';
	return xml;
}

ExportLayer = function () {
	this.frames = [];
}
ExportLayer.prototype.toXML = function (shift) {
	var spaces = shiftSpaces(shift);
	var xml = spaces + '<L>\n';
	for (var i = 0; i < this.frames.length; i++) {
		xml += this.frames[i].toXML(shift + SPACE_SHIFT_STEP);
	}
	xml += spaces + '</L>\n';
	return xml;
}

ExportAnimation = function () {
	this.name = "";
	this.startFrameIndex = 0;
	this.endFrameIndex = 0;
}
ExportAnimation.prototype.toXML = function (shift) {
	var spaces = shiftSpaces(shift);
	var xml = spaces + '<ANIM' + ' name="' + this.name + '"'
			+ ' sid="' + this.startFrameIndex + '"'
			+ ' eid="' + this.endFrameIndex + '"'
			+ ' >\n';
	xml += spaces + '</ANIM>\n';
	return xml;
}

ExportEvent = function () {
	this.frameId = 0;
	this.content = ""
}
ExportEvent.prototype.toXML = function (shift) {
	var spaces = shiftSpaces(shift);
	var xml = spaces + '<EVE'
			+ ' fid="' + this.frameId + '"'
			+ ' con="' + this.content + '"'
			+ ' >\n';
	xml += spaces + '</EVE>\n';
	return xml;
}

ExportSymbol = function () {
	this.name = "";
	this.isSocket = false;
	this.left = 0;
	this.bottom = 0; // element's top - element's height
}
ExportSymbol.prototype.toXML = function (shift) {
	var spaces = shiftSpaces(shift);
	var xml = spaces + '<S'
			+ ' name="' + this.name + '"'
			+ ' soc="' + this.isSocket + '"'
			+ ' le="' + this.left + '"'
			+ ' bo="' + this.bottom + '"'
			+ ' >\n';
	xml += spaces + '</S>\n';
	return xml;
}

ExportItem = function () {
	this.name = "";
	this.x = 0;
	this.y = 0;
	this.w = 0;
	this.h = 0;
	this.isRotation = false;
}
ExportItem.prototype.toXML = function (shift) {
	var spaces = shiftSpaces(shift);
	var xml = spaces + '<spr'
			+ ' name="' + this.name + '"'
			+ ' x="' + this.x + '"'
			+ ' y="' + this.y + '"'
			+ ' w="' + this.w + '"'
			+ ' h="' + this.h + '"'
			+ ' rot="' + this.isRotation + '"'
			+ ' >\n';
	xml += spaces + '</spr>\n';
	return xml;
}

ExportSprite = function () {
	this.version = "";
	this.frameRate = 0;
	this.frameCount = 0;
	this.anims = [];
	this.layers = [];
	this.symbols = [];
	this.events = [];
}
ExportSprite.prototype.getSymbolIndexByName = function (symbolName) {
	for (var index = 0; index < this.symbols.length; index++) {
		if (this.symbols[index].name == symbolName) {
			return index;
		}
	}
	return -1;
}
ExportSprite.prototype.toXML = function (shift) {
	var spaces = shiftSpaces(shift);
	var xml = spaces + '<Sprite' 
			+ ' version="' + this.version + '"'
			+ ' frameRate="' + this.frameRate  + '"'
			+ ' frameCount="' + this.frameCount + '"'
			+ ' >\n';
			
	var spaceStep = shiftSpaces(SPACE_SHIFT_STEP);
	
	xml += spaces + spaceStep + '<symbols>\n';
	for (var l = 0; l < this.symbols.length; l++) {
		xml += this.symbols[l].toXML(shift + SPACE_SHIFT_STEP * 2);
	}
	xml += spaces + spaceStep + '</symbols>\n';
	
	xml += spaces + spaceStep + '<anims>\n';
	for (var i = 0; i < this.anims.length; i++) {
		xml += this.anims[i].toXML(shift + SPACE_SHIFT_STEP * 2);
	}
	xml += spaces + spaceStep + '</anims>\n';
	
	xml += spaces + spaceStep + '<events>\n';
	for (var m = 0; m < this.events.length; m++) {
		xml += this.events[m].toXML(shift + SPACE_SHIFT_STEP * 2);
	}
	xml += spaces + spaceStep + '</events>\n';
	
	xml += spaces + spaceStep + '<layers>\n';
	for (var j = 0; j < this.layers.length; j++) {
		xml += this.layers[j].toXML(shift + SPACE_SHIFT_STEP * 2);
	}
	xml += spaces + spaceStep + '</layers>\n';
	
	xml += spaces + '</Sprite>\n';
	return xml;
}

ExportFilter = function () {
	/* a string that specifies the type of filter. 
	 * The value of this property determines which other properties 
	 * of the Filter object are available. The value is one of 
	 * the following: "adjustColorFilter", "bevelFilter", "blurFilter", 
	 * "dropShadowFilter", "glowFilter", "gradientBevelFilter", or "gradientGlowFilter".
	 */
	this.name = null; // "adjustColorFilter", "bevelFilter", "blurFilter", "dropShadowFilter", "glowFilter", "gradientBevelFilter", or "gradientGlowFilter"

	/* a float value that specifies the angle of the shadow or highlight color, 
	 * in degrees. Acceptable values are between 0 and 360. This property is defined 
	 * for Filter objects with a value of "bevelFilter", "dropShadowFilter", 
	 *"gradientBevelFilter", or "gradientGlowFilter" for the filter.name property.
	 */
	this.angle = null; // 0~360

	/* a float value that specifies the amount to blur in the x direction, 
	 * in pixels. Acceptable values are between 0 and 255. This property is 
	 * defined for Filter objects with a value of "bevelFilter", "blurFilter", 
	 * "dropShadowFilter", "glowFilter", "gradientBevelFilter", or "gradientGlowFilter" 
	 * for the filter.name property.
	 */
	this.blurX = null; // 0~255

	/* a float value that specifies the amount to blur in the y direction, 
	 * in pixels. Acceptable values are between 0 and 255. This property is 
	 * defined for Filter objects with a value of "bevelFilter", "blurFilter", 
	 * "dropShadowFilter", "glowFilter", "gradientBevelFilter", or "gradientGlowFilter" 
	 * for the filter.name property.
	 */
	this.blurY = null; // 0~255

	/* a float value that specifies the brightness of the filter. 
	 * Acceptable values are between -100 and 100. This property is 
	 * defined for Filter objects with a value of "adjustColorFilter" for the filter.name property.
	 */
	this.brightness = null; // -100~100

	/* the color of the filter, i
	 * A string in the format "#RRGGBB" or "#RRGGBBAA"
	 * This property is defined for Filter objects with a value of "dropShadowFilter" 
	 * or "glowFilter" for the filter.name property.
	 */
	this.color = null; // "#RRGGBB" or "#RRGGBBAA"

	/* a float value that specifies the contrast value of the filter. 
	 * Acceptable values are between -100 and 100. This property is defined for Filter 
	 * objects with a value of "adjustColorFilter" for the filter.name property.
	 */
	this.contrast = null; // -100~100

	/* a float value that specifies the distance between the 
	 * filter’s effect and an object, in pixels. Acceptable values 
	 * are from -255 to 255. This property is defined for Filter 
	 * objects with a value of "bevelFilter", "dropShadowFilter", 
	 * "gradientBevelFilter", or "gradientGlowFilter" for the filter.name property.
	 */
	this.distance = null; // -255~255

	/* a Boolean value that specifies whether the specified filter 
	 * is enabled (true) or disabled (false).
	 */
//	this.enabled = false;

	/* a Boolean value that specifies whether the source image is 
	 * hidden (true) or displayed (false). This property is defined 
	 * for Filter objects with a value of "dropShadowFilter" for the filter.name property.
	 */
	this.hideObject = null;

	/* the color of the highlight, 
	 * A string in the format "#RRGGBB" or "#RRGGBBAA"
	 * This property is defined for Filter objects with a value of "bevelFilter" for the filter.name property.
	 */
	this.highlightColor = null; // "#RRGGBB" or "#RRGGBBAA"

	/* a float value that specifies the hue of the filter. 
	 * Acceptable values are between -180 and 180. 
	 * This property is defined for Filter objects with a 
	 * value of "adjustColorFilter" for the filter.name property.
	 */
	this.hue = null; // -180~180

	/* a Boolean value that specifies whether the shadow is an inner shadow (true) 
	 * or not (false). This property is defined for Filter objects with a 
	 * value of "dropShadowFilter" or "glowFilter" for the filter.name property.
	 */
	this.inner = null;

	/* a Boolean value that specifies whether the filter is a knockout 
	 * filter (true) or not (false). This property is defined for Filter 
	 * objects with a value of "bevelFilter", "dropShadowFilter", 
	 * "glowFilter", "gradientBevelFilter", or "gradientGlowFilter" for the filter.name property.
	 */
	this.knockout = null;

	/* a string that specifies the blur quality. 
	 * Acceptable values are "low", "medium", and "high" 
	 * ("high" is similar to a Gaussian blur). This property is defined 
	 * for Filter objects with a value of "bevelFilter", "blurFilter", 
	 * "dropShadowFilter", "glowFilter", "gradientGlowFilter", or 
	 * "gradientBevelFilter" for the filter.name property.
	 */
	this.quality = null; // "low", "medium", and "high"

	/* a float value that specifies the saturation value of the filter. 
	 * Acceptable values are from -100 to 100. This property is defined 
	 * for Filter objects with a value of "adjustColorFilter" for the filter.name property.
	 */
	this.saturation = null; // -100~100

	/* the color of the shadow, 
	 * A string in the format "#RRGGBB" or "#RRGGBBAA"
	 * This property is defined for Filter objects with a value of "bevelFilter" for the filter.name property.
	 */
	this.shadowColor = null; // "#RRGGBB" or "#RRGGBBAA"

	/* an integer that specifies the percentage strength of the filter. 
	 * Acceptable values are between 0 and 25,500. 
	 * This property is defined for Filter objects with a value of "bevelFilter", 
	 * "dropShadowFilter", "glowFilter", "gradientGlowFilter", or 
	 * "gradientBevelFilter" for the filter.name property.
	 */
	this.strength = null; // 0~25500

	/* a string that specifies the type of bevel or glow. 
	 * Acceptable values are "inner", "outer", and "full". 
	 * This property is defined for Filter objects with a value of "bevelFilter", 
	 * "gradientGlowFilter", or "gradientBevelFilter" for the filter.name property.
	 */
	this.type = null; // "inner", "outer", and "full"
}
ExportFilter.prototype.toXML = function (shift) {
	var spaces = shiftSpaces(shift);
	var xml = spaces + '<Filter';
	if (this.name != null)
		xml += ' name="' + this.name + '"';
	if (this.angle != null)
		xml	+= ' angle="' + this.angle + '"';
	if (this.blurX != null)
		xml += ' blurX="' + this.blurX + '"';
	if (this.blurY != null)
		xml	+= ' blurY="' + this.blurY + '"';
	if (this.brightness != null)
		xml	+= ' brightness="' + this.brightness + '"';
	if (this.color != null)
		xml	+= ' color="' + this.color + '"';
	if (this.contrast != null)
		xml	+= ' contrast="' + this.contrast + '"';
	if (this.distance != null)
		xml	+= ' distance="' + this.distance + '"';
	if (this.hideObject != null)
		xml	+= ' hideObject="' + this.hideObject + '"';
	if (this.highlightColor != null)
		xml	+= ' highlightColor="' + this.highlightColor + '"';
	if (this.hue != null)
		xml	+= ' hue="' + this.hue + '"';
	if (this.inner != null)
		xml	+= ' inner="' + this.inner + '"';
	if (this.knockout != null)
		xml	+= ' knockout="' + this.knockout + '"';
	if (this.quality != null)
		xml	+= ' quality="' + this.quality + '"';
	if (this.saturation != null)
		xml	+= ' saturation="' + this.saturation + '"';
	if (this.shadowColor != null)
		xml	+= ' shadowColor="' + this.shadowColor + '"';
	if (this.strength != null)
		xml	+= ' strength="' + this.strength + '"';
	if (this.type != null)
		xml	+= ' type="' + this.type + '"';
		xml	+= ' >\n';
	xml += spaces + '</Filter>\n';
	return xml;
}

//------------------------------------------------


//------------ Helper ----------------

function getCustomItemType (item) {
	if (item.itemType == "bitmap") {
		return "bitmap";
	}
	else if (item.itemType == "graphic") {
		if (item.timeline.layers.length == 1 &&
			item.timeline.layers[0].frames.length == 1 && 
			item.timeline.layers[0].frames[0].duration == 1 &&
			item.timeline.layers[0].frames[0].elements.length == 1 &&
			(item.timeline.layers[0].frames[0].elements[0].elementType == "shape" || 
			item.timeline.layers[0].frames[0].elements[0].elementType == "shapeObj")) {
			return "graphic";
		}
		else {
			return "sprite";
		}
	}
	else if (item.itemType == "movie clip") {
		return "sprite";
	}
	else {
		return "other";
	}
}

function stringReplace (source, find, replace) {
	if (!source || !source.length)
		return '';
	//return source.replace(find, replace);
	return source.split(find).join(replace);
}

function saveXMLToFile (contents, fileURL, fileEnding) {
	if (!contents)
		return false;
		
	var ending = fileURL.slice(-fileEnding.length);
	if (ending != fileEnding)
		fileURL += fileEnding;
	
	var contentsLinebreaks = stringReplace(contents, "\n", "\r\n");
	
	// create folder before write file
	var foundIndex = fileURL.lastIndexOf("/");
	if (foundIndex != -1) {
		var folderURL = fileURL.substring(0, foundIndex);
		FLfile.createFolder(folderURL);
	}
	if (!FLfile.write(fileURL, contentsLinebreaks))
	{
		return false;
	}
	
	return true;
}

function shiftSpaces (shift) {
	var spaces = "";
	for (var s = 0; s < shift; s++) {
		spaces += " ";
	}
	return spaces;
}

function getItemIndexByName (itemName) {
	for (var index = 0; index < exportItems.length; index++) {
		if (exportItems[index].name == itemName) {
			return index;
		}
	}
	return -1;
}

function nextPOT (x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	return x + 1;
}

// convert Hex string to color value
function cutHex(h) { return (h.charAt(0)=="#") ? h.substring(1,7) : h }
function hexToR(h) { return parseInt((cutHex(h)).substring(0, 2), 16) }
function hexToG(h) { return parseInt((cutHex(h)).substring(2, 4), 16) }
function hexToB(h) { return parseInt((cutHex(h)).substring(4, 6), 16) }

//------------------------------------


//------------- Exporter -----------------

function buildKeyFrame (doc, timeline, layer, j, spriteData, lastKeyFrame) {
	fl.trace("    export key frame[" + j + "]");
	var keyFrame = new ExportKeyFrame();
	keyFrame.id = j;
	if (layer.frames[j].tweenType == "motion") {
		keyFrame.isMotion = true;
	}
	else {
		keyFrame.isMotion = false;
	}

	var frame = layer.frames[j];
	for (var k = 0; k < frame.elements.length; k++) {
		if (frame.elements[k].elementType == "instance" &&
			(frame.elements[k].instanceType == "bitmap" || frame.elements[k].instanceType == "symbol")) {

			if (SKIP_EXPORT_BITMAP && 
				frame.elements[k].instanceType == "bitmap") {
				//NEEDFIX: wrong position when using transform
				fl.trace("WARNING: " + layer.name + "[" + j + "][" + k + "] is a bitmap instance");
				alert("WARNING: " + layer.name + "[" + j + "][" + k + "] is a bitmap instance");
			}
			else {
				// export element
				var element = frame.elements[k];
				fl.trace("      export element[" + k + "]");
				var elementData = buildElement(element, spriteData, lastKeyFrame);
				keyFrame.elements.push(elementData); // array order is reverse as depth index order
			}

		}
	}

	return keyFrame;
}

function buildElement (element, spriteData, lastKeyFrame) {
	var elementData = new ExportElement();
	elementData.x = element.transformX;
	elementData.y = element.transformY;
	elementData.anchorX = element.getTransformationPoint().x;
	elementData.anchorY = element.getTransformationPoint().y;
	elementData.scaleX = element.scaleX;
	elementData.scaleY = element.scaleY;
	// elementData.rotation = element.rotation;
	elementData.skewX = element.skewX;
	elementData.skewY = element.skewY;
	// elementData.depth = element.depth;

	if (element.colorMode == "brightness") {
		var brightnessPercent = element.brightness / 100.0;
		var brightnessValue = Math.round(brightnessPercent * 255);

		if (element.brightness >= 0) {
			elementData.color_alpha_percent = 1.0;
			elementData.color_alpha_amount = 0;
			elementData.color_red_percent = 1.0 + brightnessPercent;
			elementData.color_red_amount = 0;
			elementData.color_green_percent = 1.0 + brightnessPercent;
			elementData.color_green_amount = 0;
			elementData.color_blue_percent = 1.0 + brightnessPercent;
			elementData.color_blue_amount = 0;
		}
		else {
			elementData.color_alpha_percent = 1.0;
			elementData.color_alpha_amount = 0;
			elementData.color_red_percent = 1.0 - brightnessPercent;
			elementData.color_red_amount = brightnessValue;
			elementData.color_green_percent = 1.0 - brightnessPercent;
			elementData.color_green_amount = brightnessValue;
			elementData.color_blue_percent = 1.0 - brightnessPercent;
			elementData.color_blue_amount = brightnessValue;
		}
	}
	else if (element.colorMode == "tint") {
		var tintPercent = element.tintPercent / 100.0;
		var tintR = hexToR(element.tintColor);
		var tintG = hexToG(element.tintColor);
		var tintB = hexToB(element.tintColor);

		elementData.color_alpha_percent = 1.0;
		elementData.color_alpha_amount = 0;
		elementData.color_red_percent = 1.0 - tintPercent;
		elementData.color_red_amount = Math.round(tintR * tintPercent);
		elementData.color_green_percent = 1.0 - tintPercent;
		elementData.color_green_amount = Math.round(tintG * tintPercent);
		elementData.color_blue_percent = 1.0 - tintPercent;
		elementData.color_blue_amount = Math.round(tintB * tintPercent);
	}
	else if (element.colorMode == "advanced") {
		elementData.color_alpha_percent = element.colorAlphaPercent / 100.0;
		elementData.color_alpha_amount = element.colorAlphaAmount;
		elementData.color_red_percent = element.colorRedPercent / 100.0;
		elementData.color_red_amount = element.colorRedAmount;
		elementData.color_green_percent = element.colorGreenPercent / 100.0;
		elementData.color_green_amount = element.colorGreenAmount;
		elementData.color_blue_percent = element.colorBluePercent / 100.0;
		elementData.color_blue_amount = element.colorBlueAmount;
	}
	else if (element.colorMode == "alpha") {
		elementData.color_alpha_percent = element.colorAlphaPercent / 100.0;
	}

	// export symbol data
	var symbolIndex = buildSymbol(element, spriteData);

	if (spriteData.symbols[symbolIndex].isSocket == true) {
		if (element.loop) { // graphic symbol in flash
			elementData.loopMode = element.loop;
			elementData.frameOffset = element.firstFrame;
		}
		else { // movie clip symbol in flash

			// !!! if there is a same movie clip symbol in last key frame, 
			//     then new element will inherit the frame offset from it.
			// !!! inherit order is same as elements array order
			elementData.loopMode = "none";
			elementData.frameOffset = 0;
			if (lastKeyFrame) {
				for (var lei = 0; lei < lastKeyFrame.elements.length; lei++) {
					if (lastKeyFrame.elements[lei].symbolIndex == symbolIndex &&
						lastKeyFrame.elements[lei].loopMode == "none" &&
						lastKeyFrame.elements[lei].frameOffsetInherited == false) {

						lastKeyFrame.elements[lei].frameOffsetInherited = true;
						elementData.frameOffset = lastKeyFrame.elements[lei].frameOffset + lastKeyFrame.duration;

						break;
					}
				}
			}

		}

		if (!SKIP_EXPORT_FILTER) {
			buildFilter(elementData, element);
		}
	}
	elementData.symbolIndex = symbolIndex;

	return elementData;
}

function buildFilter (elementData, instance) {
	if (!instance.filters)
		return;

	for (var f = 0; f < instance.filters.length; f++) {
		if (instance.filters[f].enabled == false)
			continue;

		fl.trace("          export filter[" + instance.filters[f].name + "]");

		var filterData = new ExportFilter();
		if (instance.filters[f].name != undefined)
			filterData.name = instance.filters[f].name;
		if (instance.filters[f].angle != undefined)
			filterData.angle = instance.filters[f].angle;
		if (instance.filters[f].blurX != undefined)
			filterData.blurX = instance.filters[f].blurX;
		if (instance.filters[f].blurY != undefined)
			filterData.blurY = instance.filters[f].blurY;
		if (instance.filters[f].brightness != undefined)
			filterData.brightness = instance.filters[f].brightness;
		if (instance.filters[f].color != undefined)
			filterData.color = instance.filters[f].color;
		if (instance.filters[f].contrast != undefined)
			filterData.contrast = instance.filters[f].contrast;
		if (instance.filters[f].distance != undefined)
			filterData.distance = instance.filters[f].distance;
		// if (instance.filters[f].enabled != undefined)
		// 	filterData.enabled = instance.filters[f].enabled;
		if (instance.filters[f].hideObject != undefined)
			filterData.hideObject = instance.filters[f].hideObject;
		if (instance.filters[f].hue != undefined)
			filterData.hue = instance.filters[f].hue;
		if (instance.filters[f].inner != undefined)
			filterData.inner = instance.filters[f].inner;
		if (instance.filters[f].knockout != undefined)
			filterData.knockout = instance.filters[f].knockout;
		if (instance.filters[f].quality != undefined)
			filterData.quality = instance.filters[f].quality;
		if (instance.filters[f].saturation != undefined)
			filterData.saturation = instance.filters[f].saturation;
		if (instance.filters[f].shadowColor != undefined)
			filterData.shadowColor = instance.filters[f].shadowColor;
		if (instance.filters[f].strength != undefined)
			filterData.strength = instance.filters[f].strength;
		if (instance.filters[f].type != undefined)
			filterData.type = instance.filters[f].type;
		elementData.filters.push(filterData);
	}
}

function buildSymbol (element, spriteData) {
	var symbolName = element.libraryItem.name;
	var symbolIndex = spriteData.getSymbolIndexByName(symbolName);

	if (symbolIndex == -1) {
		fl.trace("        export symbol: [" + symbolName + "] ");
		var symbolData = new ExportSymbol();
		symbolData.name = symbolName;

		var itemCustomType = getCustomItemType(element.libraryItem);
		if (itemCustomType == "sprite") {
			symbolData.isSocket = true;
		}
		else if (itemCustomType == "bitmap") {
			symbolData.isSocket = false;

			// assume that zero point at the center of bitmap
			symbolData.left = -element.libraryItem.hPixels / 2;
			symbolData.bottom = element.libraryItem.vPixels / 2;
		}
		else if (itemCustomType == "graphic") {
			symbolData.isSocket = false;

			symbolData.left = element.libraryItem.timeline.layers[0].frames[0].elements[0].left;
			symbolData.bottom = element.libraryItem.timeline.layers[0].frames[0].elements[0].top +
						element.libraryItem.timeline.layers[0].frames[0].elements[0].height;
		}
		
		symbolIndex = spriteData.symbols.push(symbolData) - 1;
	}

	return symbolIndex;
}

function buildSprite (doc, spriteItem) {
	var spriteName = spriteItem.name;
	var timeline = spriteItem.timeline;

	// init sprite data
	var spriteData = new ExportSprite();
	spriteData.version = EXPORTER_VERSION;
	spriteData.frameRate = doc.frameRate;
	exportSprites[spriteName] = spriteData;

	// export animation data
	if (timeline.layers[0] && timeline.layers[0].name == ANIM_TAGS_LAYER_NAME) {
		var lastAnim = null;
		var ind = 0
		for (ind = 0; ind < timeline.layers[0].frames.length; ind++) {
			if (timeline.layers[0].frames[ind].startFrame == ind &&
				timeline.layers[0].frames[ind].name.length > 0) { // is a named key frame
				if (lastAnim == null) {
					// new animation
					lastAnim = new ExportAnimation();
					lastAnim.startFrameIndex = ind;
					lastAnim.name = timeline.layers[0].frames[ind].name;
				}
				else {
					// end new animation
					lastAnim.endFrameIndex = ind - 1;
					spriteData.anims.push(lastAnim);
					fl.trace("  export animation: " + lastAnim.name + 
						" [" + lastAnim.startFrameIndex + ":" + lastAnim.endFrameIndex + "]");

					// start new animation
					lastAnim = new ExportAnimation();
					lastAnim.startFrameIndex = ind;
					lastAnim.name = timeline.layers[0].frames[ind].name;
				}
			}
		}
		if (lastAnim != null) { // end new animation
			lastAnim.endFrameIndex = ind - 1;
			spriteData.anims.push(lastAnim);
			fl.trace("  export animation: " + lastAnim.name + 
						" [" + lastAnim.startFrameIndex + ":" + lastAnim.endFrameIndex + "]");
		}
	}

	fl.trace("");

	// export event data
	if (timeline.layers[1] && timeline.layers[1].name == EVENT_TAGS_LAYER_NAME) {
		for (var m = 0; m < timeline.layers[1].frames.length; m++) {
			if (timeline.layers[1].frames[m].startFrame == m &&
				timeline.layers[1].frames[m].name.length > 0) { // is a named key frame
				var event = new ExportEvent();
				event.frameId = m;
				event.content = timeline.layers[1].frames[m].name;
				spriteData.events.push(event);
				fl.trace("  export event: [" + event.frameId + "] " + event.content);
			}
		}
	}

	fl.trace("");

	// export layer data
	var maxFrameCount = 0;
	var elementCount = 0;
	for (var i = 0; i < timeline.layers.length; i++) {
		if (timeline.layers[i].name == ANIM_TAGS_LAYER_NAME) {
			continue;
		}
		else if (timeline.layers[i].name == EVENT_TAGS_LAYER_NAME) {
			continue;
		}
		else if (timeline.layers[i].layerType == "guide") {
			continue;
		}
		else if (timeline.layers[i].layerType == "mask") {
			continue;
		}
		else if (timeline.layers[i].layerType == "masked") {
			continue;
		}
		else if (timeline.layers[i].layerType == "folder") {
			continue;
		}

		// the type of this layer is normal or guided(cooked)
		var layer = timeline.layers[i];
		if (layer.frameCount > maxFrameCount)
			maxFrameCount = layer.frameCount;
		fl.trace("  export layer: " + layer.name);
		var layerData = new ExportLayer();

		var lastKeyFrame = null;
		var j = 0;
		for (j = 0; j < layer.frames.length; j++) {
			// export key frame data
			if (layer.frames[j].startFrame == j) { // is key frame
				if (lastKeyFrame == null) { // begin key frame
					lastKeyFrame = buildKeyFrame(doc, timeline, layer, j, spriteData, lastKeyFrame);
				}
				else { // end key frame
					lastKeyFrame.duration = j - lastKeyFrame.id;

					layerData.frames.push(lastKeyFrame);
					elementCount += lastKeyFrame.elements.length;

					fl.trace("");

					lastKeyFrame = buildKeyFrame(doc, timeline, layer, j, spriteData, lastKeyFrame);
				}
			}
		} // export key frame END
		if (lastKeyFrame != null) {
			lastKeyFrame.duration = j - lastKeyFrame.id;

			layerData.frames.push(lastKeyFrame);
			elementCount += lastKeyFrame.elements.length;
		}

		if (layerData.frames.length > 0) {
			// ignore whole empty frames layer
			for (var fi = 0; fi < layerData.frames.length; fi++) {
				if (layerData.frames[fi].elements.length > 0) {
					spriteData.layers.push(layerData);
					break;
				}
			}
		}

		fl.trace("");
	} // export layer data END
	spriteData.frameCount = maxFrameCount;

	// warning empty sprite item
	if (elementCount == 0) {
		fl.trace('WARNING: "' + spriteName + '" is empty sprite');
		alert('WARNING: "' + spriteName + '" is empty sprite');
	}

	return spriteData;
}

function buildItem (item, itemCustomType) {
	var itemIndex = getItemIndexByName(item.name);
	if (itemIndex == -1) {
		var itemData = new ExportItem();

		itemData.name = item.name;

		return itemData;
	}

	return null;
}

function exportSpriteFile (spriteData, folderURL, fileName) {
	var xml = '<?xml version="1.0"?>\n'
			+ '<!-- Generated by Hierarchies Sprite Animation Exporter version ' + EXPORTER_VERSION + ' -->\n'
			+ spriteData.toXML(0);

	var fileURL = folderURL + "/" + fileName;
	if (saveXMLToFile(xml, fileURL, ".hanims") == false) {
		alert("save sprite (" + fileName + ") failed");
	}
}

function exportItemFile (origLib, itemDataList, folderURL, fileName) {
	if (EXPORT_PACK_IMAGE_SHEET) {
		// if (SpriteSheetExporter) { // use sprite sheet exporter with Flash CS6 later
		if (false) { //!!! there are some bug on Flash Mac version: can not export on Mac, so skip it on Mac
			exportItemFileWithSpriteSheetExporter(origLib, itemDataList, folderURL, fileName);
		}
		else {
			exportItemFileWithDIY(origLib, itemDataList, folderURL, fileName);
		}
	}
	else {
		exportItemFileWithSeparateImage(origLib, itemDataList, folderURL, fileName);
	}
}

function exportItemFileWithSpriteSheetExporter (origLib, itemDataList, folderURL, fileName) {
	var spriteSheetExporter = fl.spriteSheetExporter();
	spriteSheetExporter.layoutFormat = "HierarchiesSpriteSheet";

	for (var i = 0; i < itemDataList.length; i++) {
		var itemData = itemDataList[i];
		var itemIndex = origLib.findItemIndex(itemData.name);
		var targetItem = origLib.items[itemIndex];

		var customItemType = getCustomItemType(targetItem);
		if (customItemType == "bitmap") {
			fl.trace("---- export separate image: " + itemData.name);
			spriteSheetExporter.addBitmap(targetItem);
		}
		else {
			fl.trace("---- export separate image: " + itemData.name);
			spriteSheetExporter.addSymbol(targetItem);
		}
	}

	spriteSheetExporter.exportSpriteSheet(folderURL + "/" + fileName, 
		{format:"png", bitDepth:32, backgroundColor:"#00000000"}, false);
}

function exportItemFileWithDIY (origLib, itemDataList, folderURL, fileName) {
	var targetDoc = fl.createDocument();
	var currentWidth = ITEMS_SPACING;
	var currentHeight = ITEMS_SPACING;
	var heightMax = 0; // the max height in row
	var spriteSheet = "";

	for (var i = 0; i < itemDataList.length; i++) {
		var itemData = itemDataList[i];

		// export sprite sheet image frame 
		// (copy symbol item to new document then export new document's scene to image)
		var itemIndex = origLib.findItemIndex(itemData.name);
		var targetItem = origLib.items[itemIndex];
		targetDoc.addItem({x : 0, y : 0}, targetItem);
		var lastElement = targetDoc.getTimeline().layers[0].frames[0].elements[i];

		if (currentWidth + ITEMS_SPACING * 3 + lastElement.width > ITEMS_ATLAS_MAX_WIDTH) {
			currentWidth = ITEMS_SPACING;
			currentHeight += heightMax + ITEMS_SPACING * 2;
			heightMax = 0;
		}

		lastElement.x = lastElement.x + currentWidth - lastElement.left;
		lastElement.y = lastElement.y + currentHeight - lastElement.top;

		currentWidth += lastElement.width + ITEMS_SPACING;
		if (lastElement.height > heightMax)
			heightMax = lastElement.height;

		// update sprite sheet data
		itemData.x = lastElement.left;
		itemData.y = lastElement.top;
		itemData.w = lastElement.width;
		itemData.h = lastElement.height;

		spriteSheet += itemData.toXML(4);
	}

	var atlasWidth = Math.round(ITEMS_ATLAS_MAX_WIDTH);
	var atlasHeight = nextPOT(Math.round(currentHeight + heightMax + ITEMS_SPACING));

	var imageFileURL = folderURL + "/" + fileName + ".png";

	// save sprite sheet file
	var xml = '<?xml version="1.0"?>\n'
			+ '<!-- Generated by Hierarchies Sprite Animation Exporter version ' + EXPORTER_VERSION + ' -->\n'
			+ '<img name="' + imageFileURL.match(/[^\/]+$/) 
			+ '" w="' + atlasWidth 
			+ '" h="' + atlasHeight + '">\n'
			+ spriteSheet
			+ '</img>\n';

	var fileURL = folderURL + "/" + fileName;
	if (saveXMLToFile(xml, fileURL, ".hsheet") == false) {
		alert("save item sprite sheet failed");
	}

	// save image file
	targetDoc.width = atlasWidth;
	targetDoc.height = atlasHeight;
	if (targetDoc.exportPNG(imageFileURL, false, true) == false) {
		alert("save item image failed");
	}
}

function exportItemFileWithSeparateImage (origLib, itemDataList, folderURL, fileName) {
	for (var i = 0; i < itemDataList.length; i++) {
		var itemData = itemDataList[i];
		var itemIndex = origLib.findItemIndex(itemData.name);
		var targetItem = origLib.items[itemIndex];

		var imageFileURL = folderURL + "/" + itemData.name + ".png";

		var customItemType = getCustomItemType(targetItem);
		if (customItemType == "bitmap") {
			targetItem.exportToFile(imageFileURL, 100);
		}
		else {
			targetItem.exportToPNGSequence(imageFileURL);
		}
	}
}

//----------------------------------------


//------------- Main -----------------

function main (skip_export_bitmap, skip_export_filter, export_pack_image_sheet) {
	SKIP_EXPORT_BITMAP = skip_export_bitmap;
	SKIP_EXPORT_FILTER = skip_export_filter;
	EXPORT_PACK_IMAGE_SHEET = export_pack_image_sheet;

	// init
	var doc = fl.getDocumentDOM(); // current document
	doc.selectNone(); // reset document selection 
	var lib = doc.library; // library of current document

	// export sprites in library first
	// as same as convert shape to graphic
	for (var iii = 0; iii < lib.items.length; iii++) {
		var item = lib.items[iii];
		var customItemType = getCustomItemType(item);
		if (customItemType == "sprite") {
			var msg = "=============== build sprite: " + item.name;
			fl.trace(msg);
			lib.editItem(item.name);
			doc.selectNone();
			var spriteData = buildSprite(doc, item);
			if (spriteData.layers.length > 0)
				exportSprites[item.name] = spriteData;
			fl.trace("============================\n");
		}
	}

	// export items in library second
	for (var iii = 0; iii < lib.items.length; iii++) {
		var item = lib.items[iii];
		var customItemType = getCustomItemType(item);
		if (!SKIP_EXPORT_BITMAP) {
			if (customItemType == "bitmap") {
				var msg = "=============== build bitmap: " + item.name;
				fl.trace(msg);
				var itemData = buildItem(item, "bitmap");
				if (itemData) {
					exportItems.push(itemData);
				}
				fl.trace("============================\n");
			}
		}
		if (customItemType == "graphic" &&
			item.name.indexOf(IGNORE_ITEM_NAME_PREFIX) == -1) {
			var msg = "=============== build graphic: " + item.name;
			fl.trace(msg);
			var itemData = buildItem(item, "graphic");
			if (itemData) {
				exportItems.push(itemData);
			}
			fl.trace("============================\n\n");
		}
	}

	alert("export finish");

	// save export files
	var folderURL = fl.browseForFolderURL("save sprites to ...");
	if (folderURL && folderURL.length) {
		// save sprite files
		for (var curSpriteName in exportSprites) {
			var curSpriteData = exportSprites[curSpriteName];

			exportSpriteFile(curSpriteData, folderURL, curSpriteName);
		}

		// save item sprite sheet file and item image
		var fileName = doc.name.slice(0, -".fla".length);
		exportItemFile(lib, exportItems, folderURL, fileName);

		alert("save finish");
	}
	else {
		alert("save failed");
	}
}

//-------------------------------------

// main(SKIP_EXPORT_BITMAP, 
// 	SKIP_EXPORT_FILTER,
// 	EXPORT_PACK_IMAGE_SHEET);
