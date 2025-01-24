function newAnimation(image, width, height, duration)
  local animation = {}
  animation.spriteSheet = image;
  animation.quads = {};

  for y = 0, image:getHeight() - height, height do
    for x = 0, image:getWidth() - width, width do
      table.insert(animation.quads, love.graphics.newQuad(x, y, width, height, image:getDimensions()))
    end
  end

  animation.duration = duration or 1
  animation.currentTime = 0

  function animation:update(dt)
    self.currentTime = (self.currentTime + dt) % self.duration
  end

  function animation:current_sprite()
    local sprite_num = math.floor(self.currentTime / self.duration * #self.quads) + 1
    return self.quads[sprite_num]
  end

  return animation
end

