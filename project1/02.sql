SELECT name
FROM Pokemon
WHERE type IN (SELECT type
               FROM Pokemon 
               GROUP BY type
               HAVING COUNT(*) >= (SELECT count(*)
                                   FROM Pokemon
                                   GROUP BY type
                                   HAVING COUNT(*) < (SELECT count(*)
                                                      FROM Pokemon
                                                      GROUP BY type DESC LIMIT 1)
                                   ORDER BY COUNT(*) DESC LIMIT 1)
              )
ORDER BY name ASC
