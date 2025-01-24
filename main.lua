height = 256
width = 320
scale = 1

function love.load()
  background = love.graphics.newImage("assets/environment/background.png")


  love.resize(love.graphics.getDimensions())
end

-- function love.update()
--   
-- end

function love.draw()
  love.graphics.scale(scale)
  love.graphics.draw(background, 0, 0)
end

function love.resize(_,h)
  scale = h / height
end
