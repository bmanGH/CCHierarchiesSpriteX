/*
 * Hierarchies Sprite Animation Preprocess
 *
 * author: bman <zx123xz321hm3@hotmail.com>
 *
 * version 0.1   9/18/2013
 */


//------------ Constant --------------

var IGNORE_ITEM_NAME_PREFIX = "IGNORE__"; 				// ignore item name prefix

var JUST_LOG_WARNING = false;							// just log warning info don't do convert if 'true'
var SKIP_ConvertShapeToGraphic = false;					// skip convert shape to graphic if 'true'
var SKIP_ConvertGuidedFramesToKeyframes = true;		// skip convert guided frames to key frames if 'true'
var SKIP_ConvertMultiLayerGraphic = true;				// skip convert multi-layer graphic if 'true'
var SKIP_ConvertMultiElementSingleFrameGraphic = false;	// skip convert multi-element single-frame graphic if 'true'

//------------------------------------


//------------ Global variable -------

var preprocessItems = {};							// preprocessed item map, key is item name
var ignoreGraphicNumber = 0;

//------------------------------------


//------------ Helper ----------------

function isMultiLayerGraphic (item) {
	if (item.itemType == "graphic") {
		var timeline = item.timeline;
		for (var l = 0; l < timeline.layers.length; l++) {
			if (timeline.layers[l].frames.length > 1 ||
				(timeline.layers[l].length == 1 && timeline.layers[l].frames[0].duration > 1)) {
				return false;
			}
		}
		
		return true;
	}
	else {
		return false;
	}
}

function getCustomItemType (item) {
	if (item.itemType == "bitmap") {
		return "bitmap";
	}
	else if (item.itemType == "graphic") {
		if (item.timeline.layers.length == 1 &&
			item.timeline.layers[0].frames.length == 1 && 
			item.timeline.layers[0].frames[0].duration == 1 &&
			item.timeline.layers[0].frames[0].elements.length > 0) {
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

function getGuidedTweeningKeyframeIndices (timeline, layer, startFrameIndex, endFrameIndex) {
	// find the index of the parent layer
	var parentLayerIndex = -1;
	for (var li = 0; li < timeline.layers.length; li++)
	{
		var theLayer = timeline.layers[li];
		if (theLayer == layer.parentLayer && theLayer.layerType == 'guide')
		{
			parentLayerIndex = li;
			break;
		}
	}
	var parentLayer = timeline.layers[parentLayerIndex];

	var list = [];
	// added the -1 to the for condition because there is no point in remembering whether the last frame is a keyframe,
	// and it contributes to a bug
	for (var frameIndex = startFrameIndex; frameIndex < endFrameIndex - 1; frameIndex++)
	{
		var frame = layer.frames[frameIndex];
		if (!frame)
			continue;

		// first check that the frame is a keyframe
		var isFirstFrame = (frameIndex == startFrameIndex);
		var isKeyframe = (isFirstFrame || frame.startFrame == frameIndex);
		if (!isKeyframe)
			continue;

		// now check that the keyframe has a motion tween
		var isTweening = frame.tweenType == 'motion';
		if (!isTweening)
			continue;

		// now check that the motion guide contains graphics at the same frame
		var parentFrame = parentLayer.frames[frameIndex];
		if (!parentFrame)
			continue;

		if (!parentFrame.elements.length)
			continue;

		// now check that the tween has an existing ending keyframe
		var lastFrameIndex = frameIndex + frame.duration;
		var lastFrame = layer.frames[lastFrameIndex];
		if (!lastFrame)
			continue;

		list[frameIndex] = frame.duration;
	}

	return list;
}

function convertGuidedFramesToKeyframes (timeline, layer_index) {
	// if there is a motion guide on this layer, convert it all to keyframes
	// so we can grab the values from them.
	var guidedTweenKeyframes = getGuidedTweeningKeyframeIndices(timeline,
																timeline.layers[layer_index], 
																0, 
																timeline.layers[layer_index].frameCount);

	timeline.currentLayer = layer_index;

	// This loop has redundancies sometimes, but is necessary because
	// we could have spans of hold (non-tweening) frames in the middle 
	for (var tfi = 0; tfi < guidedTweenKeyframes.length; tfi++)
	{
		if (!guidedTweenKeyframes[tfi])
			continue;

		var tweeningFrameIndex = tfi;
		var tweeningKeyframe = timeline.layers[layer_index].frames[tweeningFrameIndex];

		// Workaround for Flash convertToKeyframes bug with connected tweens--we have to keyframe the connected
		// chain of tweens all at once, otherwise the results are incorrect.
		var lastFrameIndex = tweeningFrameIndex; //+tweeningKeyframe.duration;
		var lastFrame = timeline.layers[layer_index].frames[lastFrameIndex];

		while (lastFrame
			   && lastFrame.tweenType == "motion"
			   && guidedTweenKeyframes[lastFrameIndex])
		{
			var nextFrameIndex = lastFrameIndex + lastFrame.duration;
			// avoid the case where the last frame of the layer has a motion tween on it,
			// in which case it has a duration of 1, but there is no keyframe after it
			if (timeline.layers[layer_index].frames[nextFrameIndex])
			{
				lastFrameIndex = nextFrameIndex;
				lastFrame = timeline.layers[layer_index].frames[lastFrameIndex];
			}
			else
			{
				break;
			}
		}

		if (lastFrameIndex > tweeningFrameIndex + 1)
		{
			if (JUST_LOG_WARNING) {
				fl.trace("WARNING: " + timeline.layers[layer_index].name + " layer from [" + 
					(tweeningFrameIndex + 1) + "] to [" + lastFrameIndex + "] is guided frames");
			}
			else {
				timeline.convertToKeyframes(tweeningFrameIndex + 1, lastFrameIndex);
			}
		}
	}
}

function convertShapeToGraphic (doc, timeline, layer, frame_index) {
	for (var k = 0; k < layer.frames[frame_index].elements.length; k++) {
		if (layer.frames[frame_index].elements[k].elementType == "shape" || 
			layer.frames[frame_index].elements[k].elementType == "shapeObj") {

			if (JUST_LOG_WARNING) {
				fl.trace("WARNING: " + layer.name + " layer [" + frame_index + "][" + k + "] is shape instance");
			}
			else {
				var origDepth = layer.frames[frame_index].elements[k].depth;

				//!!! here must select item, layer, frame and element for conver to symbol to work correct
				var layer_index = timeline.findLayerIndex(layer.name);
				timeline.currentLayer = layer_index;
				timeline.currentFrame = frame_index;
				doc.selection = [timeline.layers[layer_index].frames[frame_index].elements[k]];
				var convertedSymbolItem = doc.convertToSymbol("graphic", "", "center");
				fl.trace("---- convert shape to symbol with name:" + convertedSymbolItem.name);

				// element will be top element after convert, so move element back to orig depth
				// ^   depth 0
				// |   depth 1
				// |   depth 2
				for (var d = 0; d < origDepth; d++) {
					doc.arrange("backward");
				}
				doc.selectNone();

				return false;
			}

		}
	}

	return true;
}

//------------------------------------


//------------- Preprocesser -----------------

function preprocessSprite_convertShapeToGraphic (doc, spriteItem) {
	var timeline = spriteItem.timeline;

	// iterates layer data
	for (var i = 0; i < timeline.layers.length; i++) {
		if (timeline.layers[i].layerType == "guide") {
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

		var layer = timeline.layers[i];
		fl.trace("  preprocess layer: " + layer.name);

		for (var j = 0; j < layer.frames.length; j++) {
			// preprocess key frame data
			if (layer.frames[j].startFrame == j) { // is key frame
				// convert shape to symbol in key frame
				if (convertShapeToGraphic(doc, timeline, layer, j) == false)
					return false;
			}
		} // preprocess key frame END

	} // preprocess layer data END

	return true;
}

function preprocessSprite_convertGuidedFramesToKeyframes (doc, spriteItem) {
	var timeline = spriteItem.timeline;

	// iterates layer data
	var maxFrameCount = 0;
	for (var i = 0; i < timeline.layers.length; i++) {
		if (timeline.layers[i].layerType == "guided") {
			fl.trace("---- convert guilded frames to key frames on layer: " + timeline.layers[i].name);
			convertGuidedFramesToKeyframes(timeline, i);
		}
	}

	return true;
}

function preprocessGraphic_convertMultiLayerGraphic (doc, graphicItem) {
	if (graphicItem.timeline.layers.length == 1)
		return true;

	if (JUST_LOG_WARNING) {
		fl.trace("WARNING: " + graphicItem.name + " is multi-layer graphic");
		return true;
	}

	var timeline = graphicItem.timeline;

	// delete non-display layer
	while (true) {
		var shouldContinue = false;

		for (var l = 0; l < timeline.layers.length; l++) {
			if (timeline.layers[l].layerType == "guide") {
				timeline.deleteLayer(l);
				shouldContinue = true;
			}
			else if (timeline.layers[l].layerType == "mask") {
				timeline.deleteLayer(l);
				shouldContinue = true;
			}
			else if (timeline.layers[l].layerType == "masked") {
				timeline.deleteLayer(l);
				shouldContinue = true;
			}
			else if (timeline.layers[l].layerType == "folder") {
				continue;
			}
		}

		if (shouldContinue)
			continue;
		else
			break;
	}

	doc.selectAll();
	doc.convertToSymbol("graphic", IGNORE_ITEM_NAME_PREFIX + ignoreGraphicNumber, "center");
	doc.selectNone();
	ignoreGraphicNumber++;

	// delete empty layer
	while (true) {
		var shouldContinue = false;

		for (var l = 0; l < timeline.layers.length; l++) {
			if (timeline.layers[l].frames[0].elements.length == 0) {
				timeline.deleteLayer(l);
				shouldContinue = true;
				break;
			}
		}

		if (shouldContinue)
			continue;
		else
			break;
	}

	return true;
}

function preprocessGraphic_convertMultiElementSingleFrameGraphic (doc, graphicItem) {
	if (graphicItem.timeline.layers[0].frames[0].elements.length == 1)
		return true;

	if (JUST_LOG_WARNING) {
		fl.trace("WARNING: " + graphicItem.name + " is multi-element single-frame graphic");
		return true;
	}

	doc.selectAll();
	doc.convertToSymbol("graphic", IGNORE_ITEM_NAME_PREFIX + ignoreGraphicNumber, "center");
	doc.selectNone();
	ignoreGraphicNumber++;

	return true;
}

//----------------------------------------


//------------- Main -----------------

function main (just_log_warning, 
				skip_convert_shape_to_graphic, 
				skip_convert_guided_frames_key_frames, 
				skip_convert_multi_layer_graphic,
				skip_convert_multi_element_single_frame_graphic) {
	JUST_LOG_WARNING = just_log_warning;
	SKIP_ConvertShapeToGraphic = skip_convert_shape_to_graphic;
	SKIP_ConvertGuidedFramesToKeyframes = skip_convert_guided_frames_key_frames;
	SKIP_ConvertMultiLayerGraphic = skip_convert_multi_layer_graphic;
	SKIP_ConvertMultiElementSingleFrameGraphic = skip_convert_multi_element_single_frame_graphic;

	// init
	var doc = fl.getDocumentDOM(); // current document
	doc.selectNone(); // reset document selection 
	var lib = doc.library; // library of current document

	//!!! the order of converter is important

	if (!SKIP_ConvertShapeToGraphic) { // preprocess with convert shapde to graphic
		fl.trace("convert shape to graphic start");
		preprocessItems = {};

		while (true) {
			var shouldContinue = false;

			for (var iii = 0; iii < lib.items.length; iii++) {
				var item = lib.items[iii];
				var customItemType = getCustomItemType(item);
				if (customItemType == "sprite" && !preprocessItems[item.name]) {
					var msg = "=============== preprocess sprite: " + item.name;
					fl.trace(msg);
					lib.editItem(item.name);
					doc.selectNone();
					preprocessItems[item.name] = preprocessSprite_convertShapeToGraphic(doc, item);
					fl.trace("============================\n");
					shouldContinue = true;
					break;
				}
			}

			if (shouldContinue)
				continue;
			else
				break;
		}

		fl.trace("convert shape to graphic finish");
	}

	if (!SKIP_ConvertGuidedFramesToKeyframes) { // preprocess with convert guided frames to key frames
		fl.trace("convert guilded frames to key frames start");
		preprocessItems = {};

		while (true) {
			var shouldContinue = false;

			for (var iii = 0; iii < lib.items.length; iii++) {
				var item = lib.items[iii];
				var customItemType = getCustomItemType(item);
				if (customItemType == "sprite" && !preprocessItems[item.name]) {
					var msg = "=============== preprocess sprite: " + item.name;
					fl.trace(msg);
					lib.editItem(item.name);
					doc.selectNone();
					preprocessItems[item.name] = preprocessSprite_convertGuidedFramesToKeyframes(doc, item);
					fl.trace("============================\n");
					shouldContinue = true;
					break;
				}
			}

			if (shouldContinue)
				continue;
			else
				break;
		}

		fl.trace("convert guilded frames to key frames finish");
	}

	if (!SKIP_ConvertMultiLayerGraphic) { // preprocess with convert multi-layer graphic to single layer graphic
		fl.trace("convert multi-layer graphic to single layer graphic start");
		preprocessItems = {};

		while (true) {
			var shouldContinue = false;

			for (var iii = 0; iii < lib.items.length; iii++) {
				var item = lib.items[iii];
				var customItemType = getCustomItemType(item);
				if ((customItemType == "graphic" || customItemType == "sprite") &&
					item.name.indexOf(IGNORE_ITEM_NAME_PREFIX) == -1 &&
					!preprocessItems[item.name] &&
					isMultiLayerGraphic(item)) {
					var msg = "=============== preprocess graphic: " + item.name;
					fl.trace(msg);
					lib.editItem(item.name);
					doc.selectNone();
					preprocessItems[item.name] = preprocessGraphic_convertMultiLayerGraphic(doc, item);
					fl.trace("============================\n\n");
					shouldContinue = true;
					break;
				}
			}

			if (shouldContinue)
				continue;
			else
				break;
		}

		fl.trace("convert multi-layer graphic to single layer graphic finish");
	}

	if (!SKIP_ConvertMultiElementSingleFrameGraphic) { // preprocess with convert multi-element single-frame graphic
		fl.trace("convert multi-element single-frame graphic start");
		preprocessItems = {};

		while (true) {
			var shouldContinue = false;

			for (var iii = 0; iii < lib.items.length; iii++) {
				var item = lib.items[iii];
				var customItemType = getCustomItemType(item);
				if (customItemType == "graphic" &&
					item.name.indexOf(IGNORE_ITEM_NAME_PREFIX) == -1 &&
					!preprocessItems[item.name]) {
					var msg = "=============== preprocess graphic: " + item.name;
					fl.trace(msg);
					lib.editItem(item.name);
					doc.selectNone();
					preprocessItems[item.name] = preprocessGraphic_convertMultiElementSingleFrameGraphic(doc, item);
					fl.trace("============================\n\n");
					shouldContinue = true;
					break;
				}
			}

			if (shouldContinue)
				continue;
			else
				break;
		}

		fl.trace("convert multi-element single-frame graphic finish");
	}

	alert("preprocess finish");
}

//-------------------------------------

// main (JUST_LOG_WARNING, 
// 	SKIP_ConvertShapeToGraphic, 
// 	SKIP_ConvertGuidedFramesToKeyframes, 
// 	SKIP_ConvertMultiLayerGraphic, 
// 	SKIP_ConvertMultiElementSingleFrameGraphic);
