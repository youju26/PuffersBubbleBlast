require("lib/spritesheet")
require("lib/bubble")

height = 256
width = 320
scale = 1
puffy_x = 0
puffy_y = 0
puffy_x_static = 350

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
  puffy_x = width / 2
  puffy_y = height / 2
  puffy_speed_x = 0
  puffy_speed_y = 0
  puffy_speed = 100
  mouse_x = 0
  mouse_y = 0
  puffy_angle = 0
  dx = 0
  dy = 0
end

function love.update(dt)
  mouse_x, mouse_y = love.mouse.getPosition()
  mouse_x = mouse_x
  mouse_y = mouse_y
  dx = mouse_x - puffy_x_static
  dy = mouse_y - puffy_y
  puffy_angle = (math.atan2(dy, dx) + math.pi) % (2 * math.pi)
  if love.mouse.isDown({1,2}) then
    local speed = 100
    puffy_speed_x = puffy_speed_x + math.cos(puffy_angle) * speed * dt
    puffy_speed_y = puffy_speed_y + math.sin(puffy_angle) * speed * dt
  else
      puffy_speed_x = puffy_speed_x * .99
      puffy_speed_y = puffy_speed_y * .99
  end
  puffy_x = (puffy_x + puffy_speed_x * dt)
  puffy_y = (puffy_y + puffy_speed_y * dt)
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
      love.graphics.draw(puffy.spriteSheet, puffy.quads[1], puffy_x_static, puffy_y, puffy_angle + math.pi, 1, 1, 100, 100)
    else
      love.graphics.draw(puffy.spriteSheet, puffy.quads[2], puffy_x_static, puffy_y, puffy_angle + math.pi, 1, 1, 100, 100)
  end

  for _, bubble in ipairs(bubbles) do
    bubble:draw()
  end

  -- Debug
  love.graphics.origin()
  love.graphics.setColor(1, 1, 1)
  love.graphics.print(table.concat({
      'puffy_angle: '..puffy_angle,
      'puffy_x: '..puffy_x,
      'puffy_y: '..puffy_y,
      'puffy_speed_x: '..puffy_speed_x,
      'puffy_speed_y: '..puffy_speed_y,
      'mouse_x: '..mouse_x,
      'mouse_y: '..mouse_y,
      'dx: '..dx,
      'dy: '..dy,
  }, '\n'))
end

function love.resize(_,h)
  scale = h / height
end
