require 'rubygems'
require 'compass'
require 'haml'
require 'verb_rubyd'

class VerbRubydHandler
  
  def fixDocRoot(path)
    `sudo /www/plesk_rest/current/fix_document_root.rb #{path}`
    1
  end
  
  def get_line(exception)
    return exception.message.scan(/:(\d+)/).first.first if exception.is_a?(::SyntaxError)
    exception.backtrace[0].scan(/:(\d+)/).first.first
  end
  
  def haml(text)
    begin
      engine = Haml::Engine.new(text)
      engine.render
    rescue Haml::SyntaxError => e
      raise VerbSyntaxError.new("Haml Syntax Error on line <span class='c'>#{get_line(e)}</span>: #{e.message}")
    end
  end
  
  def sass(text, load_path)
    begin
      options = Compass.sass_engine_options
      options[:load_paths] << load_path 
      options[:style] = :compressed
      engine = Sass::Engine.new(text, options)
      engine.render
    rescue Sass::SyntaxError => e
      raise VerbSyntaxError.new("Sass Syntax Error on line <span class='c'>#{get_line(e)}</span>: #{e.message}")
    end
  end
  
  def ping
    1
  end
  
end