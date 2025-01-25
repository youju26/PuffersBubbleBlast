require("lib/spritesheet")
require("lib/bubble")

height = 256
width = 320
scale = 1
puffy_x = 0

function love.load()
  background = love.graphics.newImage("assets/environment/background.png")
  background:setWrap('repeat', 'repeat')
  midground = love.graphics.newImage("assets/environment/midground.png")
  midground:setWrap('repeat', 'repeat')
  puffy = newAnimation(love.graphics.newImage("assets/puffy.png"), 200, 200, 1.0)

  bubbles = {}

  local width, height = love.graphics.getDimensions()
  local background_width, background_height = background:getDimensions()
  local aspect_ratio = background_width / background_height
  love.window.setMode(height * aspect_ratio, height, {borderless = true})
  love.resize(love.graphics.getDimensions())
  is_puffy_blow = false
end

function love.update(dt)
  puffy_x = puffy_x + 1
  for _, bubble in ipairs(bubbles) do
    bubble:update(dt)
  end
end

function love.mousepressed(x,y, button)
  table.insert(bubbles, newBubble(x, y , math.atan(y / x)))
end

function love.keypressed(key, scancode, isrepeat)
   if key == "escape" then
      love.event.quit()
   end
   if key == "space" then
      is_puffy_blow = true
   end
end

function love.keyreleased(key, scancode, isrepeat)
   if key == "space" then
      is_puffy_blow = false
   end
end

function love.draw()
  -- love.graphics.scale(scale)
  local width, height = love.graphics.getDimensions()
  local background_width, background_height = background:getDimensions()
  local background_quad = love.graphics.newQuad(puffy_x * 0.1, 0, background_width, background_height, background_width, background_height)
  love.graphics.draw(background, background_quad, 0, 0, 0, scale, scale)
  local midground_width, midground_height = midground:getDimensions()
  local midground_quad = love.graphics.newQuad(puffy_x * 0.5, 0, midground_width, midground_height, midground_width, midground_height)
  local midground_scale = height / midground_height
  love.graphics.draw(midground, midground_quad, 0, 0, 0, midground_scale, midground_scale)
  -- love.graphics.scale(.2)
  if is_puffy_blow then
      love.graphics.draw(puffy.spriteSheet, puffy.quads[1])
    else
      love.graphics.draw(puffy.spriteSheet, puffy.quads[2])
  end

  for _, bubble in ipairs(bubbles) do
    bubble:draw()
  end
end

function love.resize(_,h)
  scale = h / height
end
