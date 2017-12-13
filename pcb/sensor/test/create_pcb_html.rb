require 'nokogiri'

@doc = Nokogiri::HTML::DocumentFragment.parse <<-EOHTML
<style>
  svg {
    background: #000;
    margin: 20px;
  }

  svg:nth-child(2), svg:nth-child(3) {
    transform: rotate(180deg);
    -webkit-transform: rotate(180deg);
    -moz-transform: rotate(180deg);
    -o-transform: rotate(180deg);
    -ms-transform: rotate(180deg);
  }
</style>
<body>
</body>
EOHTML

path = Dir[File.join('.', '*_etch_copper_top_mirror.svg'), File.join('.', '*_etch_copper_top.svg'), File.join('.', 'heat_sensor_@_2_sides_fixed_etch_copper_bottom.svg'), File.join('.', 'heat_sensor_@_2_sides_fixed_etch_copper_bottom_mirror.svg')]

path.each_with_index do |file_path, index|
  html_doc = Nokogiri::HTML(File.open(file_path))

  svg = html_doc.css('svg')

  @doc << svg
  @doc << '<br/>' * 2 if index === 1

end

File.write('pcb.html', @doc.to_html.gsub(/black/, "white"))
