SELECT 
FROM CatchedPokemon, Trainer
WHERE owner_id = Trainer.id AND
      pid IN (SELECT after_id
              FROM Evolution
              WHERE after_id NOT IN (SELECT before_id
                                     FROM Evolution))
ORDER BY name ASC
