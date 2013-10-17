import sublime, sublime_plugin

class ConvertShaderToCStringCommand(sublime_plugin.TextCommand):
	def run(self, edit):
		if self.view.size() > 0:
			beginPoint = 0
			endPoint = self.view.size()
			fullRegion = sublime.Region(beginPoint, endPoint)
			lineEndings = self.view.settings().get('default_line_ending')
			if lineEndings == 'windows':
				lineEnding = '\r\n'
			elif lineEndings == 'mac':
				lineEnding = '\r'
			else: # linux
				lineEnding = '\n'

			newText = ""
			lines = self.view.lines(fullRegion)
			for line in lines:
				lineText = self.view.substr(line)
				newText += lineText.ljust(80) + '\\n\\' + lineEnding
			self.view.replace(edit, fullRegion, newText)
